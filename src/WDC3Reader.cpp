#include <WDC3Reader.h>

WDC3Reader::WDC3Reader(StreamReader& streamReader) : _streamReader(streamReader)
{
    auto length = _streamReader.Length();
    if(length < _headerSize)
    {
        std::cout << "Error Occured While Parsing WDC3 Header, Header to short." << std::endl;
        return;
    }

    _streamReader.Jump(0);

    auto magicNumber = _streamReader.Read<unsigned int>();
    if (magicNumber != WDC3FmtSig)
    {
        std::cout << "Error Occured While Parsing WDC3 Header, Magic doesnt Match." << std::endl;
        return;
    }

    Header = _streamReader.Read<WDC3Header>();

    if (Header.sectionsCount == 0 || Header.RecordsCount == 0)
        return;

    Sections = _streamReader.ReadArray<WDC3Section>(Header.sectionsCount);
    Meta = _streamReader.ReadArray<FieldMeta>(Header.FieldsCount);
    ColumnMeta = _streamReader.ReadArray<ColumnMetaData>(Header.FieldsCount);
  
    PalletData = std::map<int,std::vector<Int32>>();
    for (int i = 0; i < ColumnMeta.size(); i++)
    {
        if (ColumnMeta[i].CompressionType == CompressionType::Pallet || ColumnMeta[i].CompressionType == CompressionType::PalletArray)
        {
            auto length = ColumnMeta[i].AdditionalDataSize / sizeof(int);
            auto pallet = _streamReader.ReadArray<Int32>(length); 
            PalletData.emplace(i, pallet);
        }
    }

    //-- not yet optimised --
    CommonData = std::map<int, std::map<int, Int32>>();
    for (int i = 0; i < CommonData.size(); i++)
    {
        if (ColumnMeta[i].CompressionType == CompressionType::Common)
        {
            CommonData[i] = std::map<int, Int32>();
            auto entires = ColumnMeta[i].AdditionalDataSize / 8;
            for (int j = 0; j < entires; j++)
            {
                auto startOfList = reinterpret_cast<char*>(&CommonData[i][j]);
                //_streamReader.ReadBlock(startOfList, length * sizeof(int));
            } 
        }
    } 

}

std::vector<BlizzardDatabaseRow> WDC3Reader::ReadRows(VersionDefinition& versionDefinition)
{
    auto previousStringTableSize = 0;
    auto previousRecordCount = 0;
    auto recordBlockSize = 0;
    std::unique_ptr<char[]> recordDataBlock = nullptr;
    std::vector<BlizzardDatabaseRow> rows;

    auto length = _streamReader.Length();
    for (auto& section : Sections)
    {     
        _streamReader.Jump(section.FileOffset);

        if (!((Header.Flags & DB2Flags::Sparse) == DB2Flags::Sparse))
        {
            recordBlockSize = section.NumRecords * Header.RecordSize;
            recordDataBlock = _streamReader.ReadBlock(recordBlockSize);
 
            for (auto i = 0; i < section.StringTableSize;)
            {
                auto lastPosition = _streamReader.Position(); //56184
                auto string = _streamReader.ReadString();
                StringTable[i + previousStringTableSize] = string;

                i += _streamReader.Position() - lastPosition;

               //std::cout << lastPosition << " " << i << " " << string << std::endl;
            }

            previousStringTableSize += section.StringTableSize;
        }
        else
        {
            recordBlockSize = section.OffsetRecordsEndOffset - section.FileOffset;
            recordDataBlock = _streamReader.ReadBlock(recordBlockSize);

            if (_streamReader.Position() != section.OffsetRecordsEndOffset)
                std::cout << "Over/Under Read Section" << std::endl;
        }
  
        //Skip encryption
        if (section.TactKeyLookup != 0 && MemoryEmpty(recordDataBlock.get(), recordBlockSize))
        {
            previousRecordCount += section.NumRecords;
            continue;
        }

        auto indexData = _streamReader.ReadArray<int>(section.IndexDataSize / 4);
    
        if (indexData.size() > 0) {
            //fill index 0-X based on records
        }

        if (section.CopyTableCount > 0)
        {
            for (int i = 0; i < section.CopyTableCount; i++)
            {
                int index = _streamReader.Read<int>();
                int value = _streamReader.Read<int>();

                CopyData[index] = value;
            }
        }

        auto sparseDataEntries = std::vector<SparseEntry>();
        if (section.OffsetMapIDCount > 0)
        {
            // HACK unittestsparse is malformed and has sparseIndexData first
            if (Header.TableHash == 145293629)
            {
                auto streamPosition = _streamReader.Position();
                streamPosition += (4 * section.OffsetMapIDCount);
                _streamReader.Jump(streamPosition);
            }

            sparseDataEntries = _streamReader.ReadArray<SparseEntry>(section.OffsetMapIDCount);
        }

        auto referenceData = ReferenceData();
        if (section.ParentLookupDataSize > 0)
        {
            auto oldPosition = _streamReader.Position();
            referenceData.NumRecords = _streamReader.Read<unsigned int>();
            referenceData.MinId = _streamReader.Read<unsigned int>();
            referenceData.MaxId = _streamReader.Read<unsigned int>();
            
            auto entries = _streamReader.ReadArray<ReferenceEntry>(referenceData.NumRecords);
            for (int i = 0; i < entries.size(); i++)
            {
                referenceData.Entries[entries[i].Index] = entries[i].Id;
            }
        }

        if (section.OffsetMapIDCount > 0)
        {
            auto sparseIndexData = _streamReader.ReadArray<int>(section.OffsetMapIDCount);

            if (section.IndexDataSize > 0 && indexData.size() != sparseIndexData.size())
                std::cout << "m_indexData.Length != sparseIndexData.Length" << std::endl;

            indexData = sparseIndexData;
        }
         
        //Parsing Records
        auto position = 0;
        auto recordSize = Header.RecordSize;

        std::stringstream ss;
        for (int i = 0; i < section.NumRecords; i++)
        {
            auto bitReader = BitReader(recordDataBlock, recordBlockSize);
            if ((Header.Flags & DB2Flags::Sparse) == DB2Flags::Sparse)
            {
                //std::cout << "Sparse" << std::endl;
                bitReader.Position = position;
                position += sparseDataEntries[i].Size * 8;
            }
            else
            {
                bitReader.Offset = i * recordSize;
            }
     
            auto Id = section.IndexDataSize != 0 ? indexData[i] : -1;
            auto columns = versionDefinition.columnDefinitions;
            auto versionDefs = versionDefinition.versionDefinitions;
            auto row = BlizzardDatabaseRow();

            for (int def = 0; def < ColumnMeta.size(); def++)
            {
                auto definitionIndex = Id == -1 ? def : def + 1;
                auto column = versionDefs.definitions[definitionIndex];
                if(!column.IsInline)
                    column = versionDefs.definitions[definitionIndex + 1];
                  
                auto fieldMeta = Meta.at(def);
                auto columnMeta = ColumnMeta.at(def);
                auto commonData = std::map<int, Int32>();
                auto palletData = std::vector<Int32>();
               
                if (CommonData.contains(def))
                {
                    commonData = CommonData.at(def);
                }

                if (PalletData.contains(def))
                {
                    palletData = PalletData.at(def);
                }
  
                auto tablecolumn = columns.at(column.name);
                auto type = tablecolumn.type;
                auto columnValue = std::string();

                BlizzardDatabaseColumn blizzColumn = BlizzardDatabaseColumn();
                if (referenceData.Entries.contains(i))
                {
                    blizzColumn.ReferenceId = referenceData.Entries.at(i);
                }

                row.Columns.emplace(column.name, blizzColumn);

                if (StringExtenstions::Compare(type, "int"))
                {
                    if (column.arrLength > 0)
                    {
                        auto value = GetFieldArrayValue<unsigned int>(Id, bitReader, StringTable, fieldMeta, columnMeta, palletData, commonData);

                        //if (value.size() >= 2)
                           // std::cout << type << " " << (int)columnMeta.CompressionType << " " << column.name << " " << fieldMeta.Bits << " => " << value[0] << ":" << value[1] << std::endl;
                        continue;
                    }

                    long long value = 0;
                    if (column.size == 8 && column.isSigned)        
                        value = GetFieldValue<char>(Id, bitReader, StringTable, fieldMeta, columnMeta, palletData, commonData);
                    if (column.size == 8 && !column.isSigned)
                        value = GetFieldValue<unsigned char>(Id, bitReader, StringTable, fieldMeta, columnMeta, palletData, commonData);
                    if (column.size == 16 && column.isSigned)
                        value = GetFieldValue<short>(Id, bitReader, StringTable, fieldMeta, columnMeta, palletData, commonData);
                    if (column.size == 16 && !column.isSigned)
                        value = GetFieldValue<unsigned short>(Id, bitReader, StringTable, fieldMeta, columnMeta, palletData, commonData);                    
                    if (column.size == 32 && column.isSigned)
                        value = GetFieldValue<int>(Id, bitReader, StringTable, fieldMeta, columnMeta, palletData, commonData);
                    if (column.size == 32 && !column.isSigned)
                        value = GetFieldValue<unsigned int>(Id, bitReader, StringTable, fieldMeta, columnMeta, palletData, commonData);
                    if (column.size == 64 && column.isSigned)
                        value = GetFieldValue<long long>(Id, bitReader, StringTable, fieldMeta, columnMeta, palletData, commonData);
                    if (column.size == 64 && !column.isSigned)
                        value = GetFieldValue<unsigned long long>(Id, bitReader, StringTable, fieldMeta, columnMeta, palletData, commonData);
                    

                    row.Columns[column.name].Value = std::to_string(value);
                   //std::cout << type << " " << (int)columnMeta.CompressionType << " " << column.name << " " << 8 << " => " << value << std::endl;
                }

                if (StringExtenstions::Compare(type, "float"))
                {
                    if (column.arrLength > 0)
                    {
                        auto value = GetFieldArrayValue<float>(Id, bitReader, StringTable, fieldMeta, columnMeta, palletData, commonData);

                        //if (value.size() >= 2)
                            //std::cout << type << " " << (int)columnMeta.CompressionType << " " << column.name << " " << fieldMeta.Bits << " => " << value[0] << ":" << value[1] << std::endl;
                        
                        continue;
                    }
                    else
                    {
                        auto value =  GetFieldValue<float>(Id, bitReader, StringTable, fieldMeta, columnMeta, palletData, commonData);
                        //std::cout << type << " " << (int)columnMeta.CompressionType << " " << column.name << " " << 8 << " => " << value << std::endl;
                        row.Columns[column.name].Value = std::to_string(value);
                    }    
                }

                if (StringExtenstions::Compare(type, "string") || StringExtenstions::Compare(type, "locstring"))
                {
                    if (column.arrLength > 0)
                    {
                        auto recordIndex = i + previousRecordCount;
                        auto readerOffset = (recordIndex * recordSize) - (Header.RecordsCount * recordSize);
                        auto entries = GetFieldStringArrayValue(readerOffset, bitReader, StringTable, fieldMeta, columnMeta, palletData, commonData);
                        continue;
                    }

                    if ((Header.Flags & DB2Flags::Sparse) == DB2Flags::Sparse)
                    {
                        auto value = bitReader.ReadNullTermintingString();
                        row.Columns[column.name].Value = value;

                        //std::cout << type << " " << (int)columnMeta.CompressionType << " " << column.name << " " << fieldMeta.Bits << " => " << value << std::endl;
                    }
                    else
                    {
                        auto recordIndex = i + previousRecordCount;
                        auto readerOffset = (recordIndex * recordSize) - (Header.RecordsCount * recordSize);
                        auto offsetPosition = readerOffset + (bitReader.Position >> 3);
                        auto lookupId = GetFieldValue<int>(Id, bitReader, StringTable, fieldMeta, columnMeta, palletData, commonData);
                        auto stringLookupIndex = offsetPosition + (int)lookupId;
                        auto value = StringTable.at(stringLookupIndex);
                        row.Columns[column.name].Value = value;

                        //std::cout << type << " " << (int)columnMeta.CompressionType << " " << column.name << " " << fieldMeta.Bits << " => " << value << std::endl;
                    }       
                }
            }

            rows.push_back(row);
        }

    
        std::cout << ss.str() << std::endl;

        previousRecordCount += section.NumRecords;
    }

    return rows;
}

bool WDC3Reader::MemoryEmpty(char* data, size_t length)
{
    if (length == 0) return 1;
    if (*data) return 0;
    return memcmp(data, data + 1, length - 1) == 0;
}