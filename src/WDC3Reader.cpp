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

    auto previousStringTableSize = 0, previousRecordCount = 0;
    auto headerValue = header.HeaderData;
    for (auto& section : sections)
    {
        inputStream.seekg(section.FileOffset);

        if((headerValue.Flags & DB2Flags::Sparse) == headerValue.Flags)
        {

        }       
    }


}
