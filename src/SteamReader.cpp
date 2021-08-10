#include <StreamReader.h>

StreamReader::StreamReader(std::istream& stream) : _underlyingStream(stream)
{

}

std::string StreamReader::ReadString()
{
   std::string string;
   std::getline(_underlyingStream, string, '\0');
   return string;
}

std::string StreamReader::ReadString(std::size_t length)
{
    auto data = ReadBlock(length);    
    return std::string(data.get(),length);
}

std::unique_ptr<char[]> StreamReader::ReadBlock(std::size_t length)
{
    std::unique_ptr<char[]> blockArray(new char[length]);
     _underlyingStream.read(blockArray.get(),length);

    return blockArray;
}
  
std::streampos StreamReader::Position()
{
  return _underlyingStream.tellg();
}

void StreamReader::JumpEnd()
{
    _underlyingStream.seekg(0,_underlyingStream.end);
}

void StreamReader::JumpStart()
{
    _underlyingStream.seekg(0,_underlyingStream.beg);
}

void StreamReader::Jump(std::streampos position)
{
   _underlyingStream.seekg(position);
}

std::size_t StreamReader::Length()
{
    auto currentPosition = Position();
    JumpEnd();
    auto length = Position();
    Jump(currentPosition);

    return length;
}