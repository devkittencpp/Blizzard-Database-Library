#pragma once
#include<type_traits>
#include<fstream>
#include<iostream>
#include<vector>
#include<map>
#include<sstream>

enum class DB2Flags : unsigned short
{
    None = 0x0,
    Sparse = 0x1,
    SecondaryKey = 0x2,
    Index = 0x4,
    Unknown1 = 0x8,
    BitPacked = 0x10
};

inline DB2Flags operator & (DB2Flags lhs, DB2Flags rhs)
{
    using T = std::underlying_type_t <DB2Flags>;
    return static_cast<DB2Flags>(static_cast<T>(lhs) & static_cast<T>(rhs));
}



enum class CompressionType : int
{
    None = 0,
    Immediate = 1,
    Common = 2,
    Pallet = 3,
    PalletArray = 4,
    SignedImmediate = 5
};


#pragma pack(push,2)
struct WDC3HeaderData
{
    int RecordsCount;
    int FieldsCount;
    int RecordSize;
    int StringTableSize;
    unsigned int TableHash;
    unsigned int LayoutHash;
    int MinIndex;
    int MaxIndex;
    int Locale;
    DB2Flags Flags;
    unsigned short IdFieldIndex;
    int totalFieldsCount;
    int PackedDataOffset;
    int lookupColumnCount4;
    int columnMetaDataSize;
    int commonDataSize;
    int palletDataSize;
    int sectionsCount;
};

struct WDC3SectionData
{
    unsigned long long TactKeyLookup;
    int FileOffset;
    int NumRecords;
    int StringTableSize;
    int OffsetRecordsEndOffset;
    int IndexDataSize; 
    int ParentLookupDataSize; 
    int OffsetMapIDCount;
    int CopyTableCount;
};

union Int32
{
    int Int;
    short Short;
    char Byte;

    unsigned int UInt;
    unsigned short UShort;
    unsigned char UByte;

    char _rawbytes[4];
};

union Int64
{
    long long LongLong;
    long Long;
    int Int;
    short Short;
    char Byte;

    unsigned long long ULongLong;
    unsigned long ULong;
    unsigned int UInt;
    unsigned short UShort;
    unsigned char UByte;

    char _rawbytes[8];
};

struct FieldMetaData
{
    short Bits;
    short Offset;
};

struct ColumnCompressionData_Immediate
{
    int BitOffset;
    int BitWidth;
    int Flags; // 0x1 signed
};

struct ColumnCompressionData_Pallet
{
    int BitOffset;
    int BitWidth;
    int Cardinality;
};

struct ColumnCompressionData_Common
{
    int DefaultValue;
    int B;
    int C;
};

union ColumnCompressionData
{
    ColumnCompressionData_Immediate Immediate;
    ColumnCompressionData_Pallet Pallet;
    ColumnCompressionData_Common Common;
};

struct ColumnMetaData
{
    unsigned short RecordOffset; 
    unsigned short Size; 
    unsigned int AdditionalDataSize; 
    CompressionType CompressionType; 
    ColumnCompressionData compressionData;
};

union ColumnMeta
{
    ColumnMetaData HeaderData;
    char rawBytes[sizeof(ColumnMetaData)];
};

union FieldMeta
{
    FieldMetaData HeaderData;
    char rawBytes[sizeof(FieldMetaData)];
};

union WDC3Header
{
    WDC3HeaderData HeaderData;
    char rawBytes[sizeof(WDC3HeaderData)];
};

union WDC3Section
{
    WDC3SectionData SectionData;
    char rawBytes[sizeof(WDC3SectionData)];
};

#pragma pack(pop)

class WDC3Reader
{
private:
    const int _headerSize = 72;
    const unsigned int WDC3FmtSig = 0x33434457;

    std::ifstream _stream;

    std::map<int,int> _copyData;
public:
    WDC3Reader(std::ifstream& inputStream);
private:
    std::string ReadString(std::ifstream& inputStream);
    bool MemoryEmpty(char* data, size_t length);
};

