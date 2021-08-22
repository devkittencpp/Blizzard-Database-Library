#pragma once
#include<type_traits>
#include<fstream>
#include<istream>
#include<iostream>
#include<vector>
#include<map>
#include<sstream>
#include<StreamReader.h>
#include<Types.h>
#include<WDC3Row.h>
#include<FileStructures.h>

class WDC3Reader
{
private:
    const int _headerSize = 72;
    const unsigned int WDC3FmtSig = 0x33434457;
    StreamReader _streamReader;

public:
    WDC3Header Header;

    std::vector<WDC3Section> Sections;
    std::vector<FieldMeta> Meta;
    std::vector<ColumnMetaData> ColumnMeta;
    std::map<int, std::vector<Int32>> PalletData;
    std::map<int, std::map<int, Int32>> CommonData;
    std::map<int, int> CopyData;
    std::map<long, std::string> StringTable;

    WDC3Reader(StreamReader& streamReader);
    void ReadRows(VersionDefinition& versionDefinition);
private:
    bool MemoryEmpty(char* data, size_t length);
};

