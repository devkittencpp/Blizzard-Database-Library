#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <Types.h>

struct MemoryBuffer : std::streambuf
{
public:
    MemoryBuffer(char* firstElement, char* LastElement) 
    {
        this->setg(firstElement, firstElement, LastElement);
    }
};

class BitReader
{
    std::unique_ptr<char[]>& _dataStart;
public:
    int Position;
    int DataLength;
    int Offset;
    BitReader(std::unique_ptr<char[]>& dataStart, unsigned int dataLength) : _dataStart(dataStart), DataLength(dataLength), Position(0), Offset(0)
    {
      
    }

    std::string hexStr(char* data, int len)
    {
        char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

       std::stringstream ss;
       for (int i = 0; i < len; ++i)
       {
           char const byte = data[i];

           ss << hex_chars[(byte & 0xF0) >> 4];
           ss << hex_chars[(byte & 0x0F) >> 0];
       }

        return ss.str();
    }

    std::string DumpBlock()
    {
        return hexStr(_dataStart.get(), DataLength);
    }

    unsigned int ReadUint32(int numberOfBits)
    {
        auto index = Offset + (int)(Position >> (int)3);
        auto p1 = (32 - numberOfBits - (Position & 7));
        auto p2 = (32 - numberOfBits);

        auto integerPointer = reinterpret_cast<unsigned int*>(_dataStart.get());
        auto result = ((integerPointer[index / 4]) << p1 >> p2);
        Position += numberOfBits;
        return result;
    }

    unsigned long long ReadUint64(int numberOfBits)
    {
        auto index = Offset + (int)(Position >> (int)3);
        auto p1 = (64 - numberOfBits - (Position & 7));
        auto p2 = (64 - numberOfBits);

        auto integerPointer = reinterpret_cast<unsigned long*>(_dataStart.get());
        auto result = ((integerPointer[index / 8]) << p1 >> p2);
        Position += numberOfBits;
        return result;
    }

    Int64 ReadValue64(int numberOfBits)
    {
        auto value = Int64();
        value.ULongLong = ReadUint64(numberOfBits);
        return value;
    }

    Int64 ReadSignedValue64(int numberOfBits)
    {
        auto value = Int64();
        value.ULongLong = ReadUint64(numberOfBits);
        auto signedShift = 1UL << (numberOfBits - 1);
        value.ULongLong = (signedShift ^ value.ULongLong) - signedShift;
        return value;
    }

};

class StreamReader
{
private:
    std::istream& _underlyingStream;
public:
    StreamReader(std::istream& stream);

    template <typename T> T Read()
    {
        auto type = T();
        auto size = sizeof(T);    
        auto pointer = reinterpret_cast<char*>(&type);
        _underlyingStream.read(pointer, size);
        return type;
    }

    template <typename T> std::vector<T> ReadArray(std::size_t length)
    {
        auto vector = std::vector<T>(length);
        auto size = sizeof(T);
        auto pointer = reinterpret_cast<char*>(vector.data());
        _underlyingStream.read(pointer, size * length);
        return vector;
    }

    std::unique_ptr<char[]> ReadBlock(std::size_t length);
    std::string ReadString();
    std::string ReadString(std::size_t length);
    void Jump(std::streampos position);
    void JumpEnd();
    void JumpStart();
    std::size_t Length();
    std::streampos Position();
};