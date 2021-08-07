#include <WDC3Reader.h>

WDC3Reader::WDC3Reader(std::ifstream& inputStream) : _streamReader(inputStream)
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

    auto sections = _streamReader.ReadArray<WDC3Section>(Header.sectionsCount);
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
                inputStream.read(startOfList, length * sizeof(int));
            } 
        }
    } 

   
    auto previousStringTableSize = 0, previousRecordCount = 0;
    auto recordSize = 0;
    std::unique_ptr<char[]> recordDataBlock = nullptr;

    for (auto& section : sections)
    {       
        _streamReader.Jump(section.FileOffset);

        if((Header.Flags & DB2Flags::Sparse) != Header.Flags)
        {
            recordSize = section.NumRecords * Header.RecordSize;
            recordDataBlock = _streamReader.ReadBlock(recordSize);

            for(auto i = 0 ; i < section.StringTableSize;)
            {
                auto lastPosition = _streamReader.Position(); //56184
                auto string = _streamReader.ReadString();
                StringTable[i+previousStringTableSize] = string;

                i += _streamReader.Position() - lastPosition;

                std::cout << lastPosition << " " << i << " " << string << std::endl;
            }

            previousStringTableSize += section.StringTableSize;
        }
        else
        {
            recordSize = section.OffsetRecordsEndOffset - section.FileOffset;
            recordDataBlock = _streamReader.ReadBlock(recordSize);

            if (_streamReader.Position() != section.OffsetRecordsEndOffset)
                std::cout << "Over/Under Read Section" << std::endl;     
        } 

        if (section.TactKeyLookup != 0 && MemoryEmpty(recordDataBlock.get(), recordSize))
        {
            previousRecordCount += section.NumRecords;
            continue;
        }
 
        auto indexData = _streamReader.ReadArray<int>(section.IndexDataSize);
        if(indexData.size() > 0 ){
            //fill index 0-X based on records
        }
           
        if(section.CopyTableCount > 0)
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

        if (section.ParentLookupDataSize > 0)
        {         
           //var oldPosition = reader.BaseStream.Position;
           //refData.NumRecords = reader.ReadInt32();
           //refData.MinId = reader.ReadInt32();
           //refData.MaxId = reader.ReadInt32();
           //
           //var entries = reader.ReadArray<ReferenceEntry>(refData.NumRecords);
           //for (int i = 0; i < entries.Length; i++)
           //{
           //    refData.Entries[entries[i].Index] = entries[i].Id;
           //}
            
            std::cout << "Has Lookup Data" << std::endl;
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
        auto recordMemoryBuffer = MemoryBuffer(recordDataBlock.get(), recordDataBlock.get() + recordSize);
        for (int i = 0; i < section.NumRecords; i++)
        {      
            std::istream stream(&recordMemoryBuffer);
            auto streamReader = StreamReader(stream);

            if ((Header.Flags & DB2Flags::Sparse) == Header.Flags)
            {
                streamReader.Jump(0);
                position += sparseDataEntries[i].Size * 8;
            }
            else
            {
                std::cout << "Not Sparse" << std::endl;

                streamReader.Jump(i * recordSize);
            }
        }

        auto row = WDC3Row();

        previousRecordCount += section.NumRecords;
    }
}

bool WDC3Reader::MemoryEmpty(char* data, size_t length)
{
    if (length == 0) return 1;
    if (*data) return 0;
    return memcmp(data, data + 1, length - 1) == 0;
}