#pragma once
#include <iostream>
#include <vector>

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

    template <typename T> T ReadArray(std::size_t length)
    {
        auto vector = std::vector<T>(length);
        auto size = sizeof(T);
        auto pointer = reinterpret_cast<char*>(vector.data());
        _underlyingStream.read(pointer, size);

        return vector;
    }
    
    void Jump(long position);
    std::size_t Length();
};