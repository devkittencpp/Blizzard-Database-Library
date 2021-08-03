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

class WDC3Reader
{
private:
    const int _headerSize = 72;
    const unsigned int WDC3FmtSig = 0x33434457;

    StreamReader _streamReader;

    std::map<int,int> _copyData;
public:
    WDC3Reader(std::ifstream& inputStream);
private:
    bool MemoryEmpty(char* data, size_t length);
};

