#include <WDC3Reader.h>

WDC3Reader::WDC3Reader(std::ifstream& inputStream) 
{
    inputStream.seekg (0, inputStream.end);
    auto length = inputStream.tellg();
    inputStream.seekg (0, inputStream.beg);

    if(length < _headerSize)
    {
        std::cout << "Error Occured While Parsing WDC3 Header, Header to short." << std::endl;
        return;
    }

    char magicNumber[4];
    inputStream.read(magicNumber, 4);
    unsigned int magic;
    std::memcpy(&magic, magicNumber, sizeof(unsigned int));

    if (magic != WDC3FmtSig)
    {
        std::cout << "Error Occured While Parsing WDC3 Header, Magic doesnt Match." << std::endl;
        return;
    }

    auto header = WDC3Header();
    inputStream.read(header.rawBytes, 68);

    if (header.HeaderData.sectionsCount == 0 || header.HeaderData.RecordsCount == 0)
        return;

    auto sections = std::vector<WDC3SectionData>();
    for(int i = 0; i < header.HeaderData.sectionsCount; i++)
    {
        auto section = WDC3Section();
        inputStream.read(section.rawBytes, sizeof(WDC3SectionData));
        sections.push_back(section.SectionData);
    }

    auto metaData = std::vector<FieldMetaData>();
    for (int i = 0; i < header.HeaderData.FieldsCount; i++)
    {
        auto fieldMetaData = FieldMeta();
        inputStream.read(fieldMetaData.rawBytes, sizeof(FieldMetaData));
        metaData.push_back(fieldMetaData.HeaderData);
    }

    auto columnData = std::vector<ColumnMetaData>();
    for (int i = 0; i < header.HeaderData.FieldsCount; i++)
    {
        auto columnMetaData = ColumnMeta();
        inputStream.read(columnMetaData.rawBytes, sizeof(ColumnMetaData));
        columnData.push_back(columnMetaData.HeaderData);
    }
   
    auto palletData = std::map<int,std::vector<Int32>>();
    for (int i = 0; i < columnData.size(); i++)
    {
        if (columnData[i].CompressionType == CompressionType::Pallet || columnData[i].CompressionType == CompressionType::PalletArray)
        {
            auto length = columnData[i].AdditionalDataSize / sizeof(int);
            auto list = std::vector<Int32>(length);
            auto startOfList = reinterpret_cast<char*>(list.data());

            inputStream.read(startOfList, length * sizeof(int));

            palletData.emplace(i, list);
        }
    }

    auto commonData = std::map<int, std::map<int, Int32>>();
    for (int i = 0; i < columnData.size(); i++)
    {
        if (columnData[i].CompressionType == CompressionType::Common)
        {
            commonData[i] = std::map<int, Int32>();
            auto entires =  columnData[i].AdditionalDataSize / 8;
            for (int j = 0; j < entires; j++)
            {
                auto startOfList = reinterpret_cast<char*>(&commonData[i][j]);
                inputStream.read(startOfList, length * sizeof(int));
            } 
        }
    } 


    _copyData = std::map<int,int>();
    auto previousStringTableSize = 0, previousRecordCount = 0;
    auto headerValue = header.HeaderData;
    auto recordSize = 0;
    char* readBuffer = nullptr;
    auto stringTable = std::map<long,std::string>();
    for (auto& section : sections)
    {       
        if((headerValue.Flags & DB2Flags::Sparse) != headerValue.Flags)
        {
            recordSize = section.NumRecords *  headerValue.RecordSize;
            readBuffer = new char[recordSize];

            inputStream.read(readBuffer, recordSize);

            for(auto i = 0 ; i < section.StringTableSize;)
            {
                auto lastPosition = inputStream.tellg(); //56184
                auto string = ReadString(inputStream);
                stringTable[i+previousStringTableSize] = string;

                i +=  inputStream.tellg() - lastPosition; 

                std::cout << lastPosition << " " << i << " " << string << std::endl;
            }

            previousStringTableSize += section.StringTableSize;
        }
        else
        {
            recordSize = section.OffsetRecordsEndOffset - section.FileOffset;
            readBuffer = new char[recordSize];

            inputStream.read(readBuffer,recordSize);

            if (inputStream.tellg() != section.OffsetRecordsEndOffset)
                std::cout << "Over/Under Read Section" << std::endl;     
        } 

        if (section.TactKeyLookup != 0 && MemoryEmpty(readBuffer, recordSize))
        {
            previousRecordCount += section.NumRecords;
            continue;
        }

        auto indexSize = section.IndexDataSize / 4;
        auto indexData = std::vector<int>(indexSize);
        auto indexDataPointer = reinterpret_cast<char*>(indexData.data());
        inputStream.read(indexDataPointer, section.IndexDataSize);

        if(indexData.size() > 0 )
            //fill index 0-X based on records

        if(section.CopyTableCount > 0)
        {   
            for (int i = 0; i < section.CopyTableCount; i++)
            {
                int index;
                int value;

                auto indexPtr = reinterpret_cast<char*>(&index);
                auto valuePtr = reinterpret_cast<char*>(&value);

                inputStream.read(indexPtr, 4);
                inputStream.read(valuePtr, 4);

                 _copyData[index] = value;
            }
               
        }


        // delete[] readBuffer; 
    }
}
std::string WDC3Reader::ReadString(std::ifstream& inputStream)
{
   std::string string;
   std::getline(inputStream, string, '\0');
   return string;
 }

bool WDC3Reader::MemoryEmpty(char* data, size_t length)
{
    if (length == 0) return 1;
    if (*data) return 0;
    return memcmp(data, data + 1, length - 1) == 0;
}