#pragma once
#include<type_traits>
#include<fstream>
#include<istream>
#include<iostream>
#include<vector>
#include<map>
#include<sstream>
#include<StreamReader.h>
#include<bitreader.h>
#include<Types.h>
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
    std::vector<BlizzardDatabaseRow> ReadRows(VersionDefinition& versionDefinition);
private:
    bool MemoryEmpty(char* data, size_t length);
 
    template<typename T>
    T GetFieldValue(int Id, BitReader& reader, std::map<long, std::string>& stringLookup, FieldMeta& fieldMeta, 
        ColumnMetaData& columnMeta, std::vector<Int32>& palletData, std::map<int, Int32>& commonData)
    {
        switch(columnMeta.CompressionType)
        {
            case CompressionType::None:
            {
                auto bitSize = 32 - fieldMeta.Bits;
                if (bitSize <= 0)
                    bitSize = columnMeta.compressionData.Immediate.BitWidth;

                return reader.ReadValue64(bitSize).As<T>();
            }
            case CompressionType::SignedImmediate:
            {
                return reader.ReadSignedValue64(columnMeta.compressionData.Immediate.BitWidth).As<T>();
            }
            case CompressionType::Immediate:
            {
                return reader.ReadValue64(columnMeta.compressionData.Immediate.BitWidth).As<T>();
            }
            case CompressionType::Common:
            {
                if (commonData.contains(Id))
                    return commonData.at(Id).As<T>();

                return columnMeta.compressionData.Common.DefaultValue.As<T>();
            }
            case CompressionType::Pallet:
            {   
                auto value = reader.ReadUint32(columnMeta.compressionData.Pallet.BitWidth);
                return palletData[value].As<T>();   
            }
            case CompressionType::PalletArray:
            {
                if (columnMeta.compressionData.Pallet.Cardinality != 1)
                    break;

                auto palletArrayIndex = reader.ReadUint32(columnMeta.compressionData.Pallet.BitWidth);
                return palletData[palletArrayIndex].As<T>();
            }
        }

        return static_cast<T>(0);
    }

    template<typename T>
    std::vector<T> GetFieldArrayValue(int Id, BitReader& reader, std::map<long, std::string>& stringLookup, FieldMeta& fieldMeta,
        ColumnMetaData& columnMeta, std::vector<Int32>& palletData, std::map<int, Int32>& commonData)
    {
        auto vector = std::vector<T>();
        switch (columnMeta.CompressionType)
        {
            case CompressionType::None:
            {
                auto bitSize = 32 - fieldMeta.Bits;
                if (bitSize <= 0)
                    bitSize = columnMeta.compressionData.Immediate.BitWidth;

                auto entires = columnMeta.Size / bitSize;
                for (auto i = 0; i < entires; i++)
                {
                    auto entry = reader.ReadValue64(bitSize).As<T>();
                    vector.push_back(entry);
                }
                break;
            }
            case CompressionType::PalletArray:
            {
                auto cardinality = columnMeta.compressionData.Pallet.Cardinality;
                auto index = reader.ReadUint32(columnMeta.compressionData.Pallet.BitWidth);

                for (auto i = 0; i < cardinality; i++)
                {
                    auto data = palletData[i + cardinality * (index)].As<T>();
                    vector.push_back(data);
                }
            }
        }
        return vector;
    }

    std::vector<std::string> GetFieldStringArrayValue(int offset, BitReader& reader, std::map<long, std::string>& stringLookup, FieldMeta& fieldMeta,
        ColumnMetaData& columnMeta, std::vector<Int32>& palletData, std::map<int, Int32>& commonData)
    {
        auto vector = std::vector<std::string>();
        switch (columnMeta.CompressionType)
        {
            case CompressionType::None:
            {
                auto bitSize = 32 - fieldMeta.Bits;
                if (bitSize <= 0)
                    bitSize = columnMeta.compressionData.Immediate.BitWidth;

                auto entires = columnMeta.Size / bitSize;
                for (auto i = 0; i < entires; i++)
                {
                    auto entryIndex = (reader.Position >> 3) + offset + reader.ReadValue64(bitSize).As<int>();
                    auto string = stringLookup.at(entryIndex);
                    vector.push_back(string);
                }
            }
        }

        return vector;
    }
};

