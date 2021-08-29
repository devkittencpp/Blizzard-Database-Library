#include <readers/wdc3/WDC3TableReader.h>

namespace BlizzardDatabaseLib {
    namespace Reader {
  
        WDC3TableReader::WDC3TableReader(std::shared_ptr<Stream::StreamReader> streamReader, Structures::VersionDefinition versionDefinition) : _streamReader(streamReader), _versionDefinition(versionDefinition)
        {

        }

        WDC3TableReader::~WDC3TableReader()
        {
            _streamReader.reset();
        }

        void WDC3TableReader::LoadTableStructure()
        {
            auto length = _streamReader->Length();
            auto headerSize = sizeof(Structures::WDC3Header);

            if (length < headerSize)
            {
                std::cout << "Error Occured While Parsing WDC3 Header, Header to short." << std::endl;
                return;
            }

            _streamReader->Jump(0);

            auto magicNumber = _streamReader->Read<unsigned int>();
            if (magicNumber != Flag::TableFormatSignatures::WDC3_FMT_SIGNATURE)
            {
                std::cout << "Error Occured While Parsing WDC3 Header, Format Signature doesnt Match." << std::endl;
                return;
            }

            Header = _streamReader->Read<Structures::WDC3Header>();

            if (Header.sectionsCount == 0 || Header.RecordsCount == 0)
                return;

            Sections = _streamReader->ReadArray<Structures::WDC3Section>(Header.sectionsCount);
            Meta = _streamReader->ReadArray<Structures::FieldMeta>(Header.FieldsCount);
            ColumnMeta = _streamReader->ReadArray<Structures::ColumnMetaData>(Header.FieldsCount);

            PalletData = std::map<int, std::vector<Structures::Int32>>();
            for (int i = 0; i < ColumnMeta.size(); i++)
            {
                if (ColumnMeta[i].Compression == Structures::CompressionType::Pallet || ColumnMeta[i].Compression == Structures::CompressionType::PalletArray)
                {
                    auto length = ColumnMeta[i].AdditionalDataSize / sizeof(int);
                    auto pallet = _streamReader->ReadArray<Structures::Int32>(length);
                    PalletData.emplace(i, pallet);
                }
            }

            //-- not yet optimised --
            CommonData = std::map<int, std::map<int, Structures::Int32>>();
            for (int i = 0; i < CommonData.size(); i++)
            {
                if (ColumnMeta[i].Compression == Structures::CompressionType::Common)
                {
                    CommonData[i] = std::map<int, Structures::Int32>();
                    auto entires = ColumnMeta[i].AdditionalDataSize / 8;
                    for (int j = 0; j < entires; j++)
                    {
                        auto startOfList = reinterpret_cast<char*>(&CommonData[i][j]);
                        //_streamReader.ReadBlock(startOfList, length * sizeof(int));
                    }
                }
            }
        }

        Structures::BlizzardDatabaseRow WDC3TableReader::RecordById(unsigned int Id) 
        {
            std::unique_ptr<char[]> recordDataBlock = nullptr;
            std::vector<Structures::BlizzardDatabaseRow> rows;

            for (auto& section : Sections)
            {
                auto recordBlockSize = section.NumRecords * Header.RecordSize;
                auto startOfStringTable = section.FileOffset + (recordBlockSize);
                auto endOfStringTable = startOfStringTable + section.StringTableSize;

                _streamReader->Jump(section.FileOffset);

                //Read Record MemoryBlock
                if (Extension::Flag::HasFlag(Header.Flags, Flag::DatabaseVersion2Flag::VariableWidthRecord)) //If the data is Fixed record Width
                {
                    recordBlockSize = section.OffsetRecordsEndOffset - section.FileOffset;
                    recordDataBlock = _streamReader->ReadBlock(recordBlockSize);

                    if (_streamReader->Position() != section.OffsetRecordsEndOffset)
                        std::cout << "Over/Under Read Section" << std::endl;
                }
                else //If record data is not fixed width
                {
                    recordDataBlock = _streamReader->ReadBlock(recordBlockSize);
                    _streamReader->Jump(endOfStringTable);
                }

                //Skip encryption
                auto emptyMemoryBlock = Extension::Memory::IsEmpty(recordDataBlock.get(), recordBlockSize);
                if (section.TactKeyLookup != 0 && emptyMemoryBlock)
                {
                    continue;
                }

                auto indexData = _streamReader->ReadArray<int>(section.IndexDataSize / 4);
                if (indexData.size() > 0) {
                    //fill index 0-X based on records
                }

                if (section.CopyTableCount > 0)
                {
                    for (int i = 0; i < section.CopyTableCount; i++)
                    {
                        int index = _streamReader->Read<int>();
                        int value = _streamReader->Read<int>();

                        CopyData[index] = value;
                    }
                }

                auto sparseDataEntries = std::vector<Structures::SparseEntry>();
                if (section.OffsetMapIDCount > 0)
                {
                    // HACK unittestsparse is malformed and has sparseIndexData first
                    if (Header.TableHash == 145293629)
                    {
                        auto streamPosition = _streamReader->Position();
                        streamPosition += (4 * section.OffsetMapIDCount);
                        _streamReader->Jump(streamPosition);
                    }

                    sparseDataEntries = _streamReader->ReadArray<Structures::SparseEntry>(section.OffsetMapIDCount);
                }

                auto referenceData = Structures::ReferenceData();
                if (section.ParentLookupDataSize > 0)
                {
                    auto oldPosition = _streamReader->Position();
                    referenceData.NumRecords = _streamReader->Read<unsigned int>();
                    referenceData.MinId = _streamReader->Read<unsigned int>();
                    referenceData.MaxId = _streamReader->Read<unsigned int>();

                    auto entries = _streamReader->ReadArray<Structures::ReferenceEntry>(referenceData.NumRecords);
                    for (int i = 0; i < entries.size(); i++)
                    {
                        referenceData.Entries[entries[i].Index] = entries[i].Id;
                    }
                }

                if (section.OffsetMapIDCount > 0)
                {
                    auto sparseIndexData = _streamReader->ReadArray<int>(section.OffsetMapIDCount);

                    if (section.IndexDataSize > 0 && indexData.size() != sparseIndexData.size())
                        std::cout << "m_indexData.Length != sparseIndexData.Length" << std::endl;

                    indexData = sparseIndexData;
                }

                auto indexOfId = 0U;
                if (!Extension::Vector::IndexOf<int>(indexData, Id, indexOfId))
                    continue;

                auto recordSize = Header.RecordSize;
                auto bitReader = Stream::BitReader(recordDataBlock, recordBlockSize);
                auto recordReader = WDC3RecordReader(_streamReader,_versionDefinition,bitReader,Header);
                if (Extension::Flag::HasFlag(Header.Flags, Flag::DatabaseVersion2Flag::VariableWidthRecord))
                {
                    bitReader.Offset = sparseDataEntries[indexOfId].Offset - section.FileOffset;
                    return recordReader.ReadRecord(indexOfId, section, bitReader, Meta, ColumnMeta, PalletData, CommonData, referenceData, indexData);
                }
                else
                {
                    bitReader.Offset = indexOfId * recordSize;
                    return recordReader.ReadRecord(indexOfId, section, bitReader, Meta, ColumnMeta, PalletData, CommonData, referenceData, indexData);
                }
            }
            return Structures::BlizzardDatabaseRow();
        }

        std::vector<Structures::BlizzardDatabaseRow> WDC3TableReader::Records() 
        {
            return std::vector<Structures::BlizzardDatabaseRow>();
        }

        std::size_t WDC3TableReader::RecordCount()
        {
            return Header.RecordsCount;
        }
    }
}