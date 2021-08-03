#pragma once
#include <iostream>
#include <vector>
#include <string>


struct MemoryBuffer : std::streambuf
{
public:
    MemoryBuffer(char* firstElement, char* LastElement) 
    {
        this->setg(firstElement, firstElement, LastElement);
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
    void Jump(std::streampos position);
    void JumpEnd();
    void JumpStart();
    std::size_t Length();
    std::streampos Position();
};