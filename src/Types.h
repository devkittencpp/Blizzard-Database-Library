#pragma once
#include<type_traits>

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
struct WDC3Header
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

struct SparseEntry
{
    unsigned int Offset;
    unsigned short Size;
};

struct WDC3Section
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
    float Float;

    unsigned int UInt;
    unsigned short UShort;
    unsigned char UByte;

    char _rawbytes[4];

public:
    template<typename T>
    T As()
    {
        if (std::is_same<T, int>::value)
            return Int;
        if (std::is_same<T, short>::value)
            return Short;
        if (std::is_same<T, char>::value)
            return Byte;
        if (std::is_same<T, unsigned int>::value)
            return UInt;
        if (std::is_same<T, unsigned short>::value)
            return UShort;
        if (std::is_same<T, unsigned char>::value)
            return UByte;
        if (std::is_same<T, float>::value)
            return Float;
    }
};

union Int64
{
    long long LongLong;
    long Long;
    int Int;
    short Short;
    char Byte;
    float Float;

    unsigned long long ULongLong;
    unsigned long ULong;
    unsigned int UInt;
    unsigned short UShort;
    unsigned char UByte;

    char _rawbytes[8];

public:
    template<typename T>
    T As()
    {
        if (std::is_same<T, long long>::value)
            return LongLong;
        if (std::is_same<T, long>::value)
            return Long;
        if (std::is_same<T, int>::value)
            return Int;
        if (std::is_same<T, short>::value)
            return Short;
        if (std::is_same<T, char>::value)
            return Byte;
        if (std::is_same<T, unsigned long long>::value)
            return ULongLong;
        if (std::is_same<T, unsigned long>::value)
            return ULong;
        if (std::is_same<T, unsigned int>::value)
            return UInt;
        if (std::is_same<T, unsigned short>::value)
            return UShort;
        if (std::is_same<T, unsigned char>::value)
            return UByte;
        if (std::is_same<T, float>::value)
            return Float;
    }
};

struct FieldMeta
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


#pragma pack(pop)