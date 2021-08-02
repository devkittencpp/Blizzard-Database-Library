#include <StreamReader.h>

StreamReader::StreamReader(std::istream& stream) : _underlyingStream(stream)
{

}

void StreamReader::Jump(long position)
{
   _underlyingStream.seekg(position);
}

std::size_t StreamReader::Length()
{
    auto currentPosition = _underlyingStream.tellg();
    _underlyingStream.seekg(0,_underlyingStream.end);
    auto length = _underlyingStream.tellg();
    _underlyingStream.seekg(currentPosition);

    return length;
}