#include "BlizzardDatabase.h"

BlizzardDatabase::BlizzardDatabase(std::string databaseCollectionDirectory) : _databaseFilesLocation(databaseCollectionDirectory)
{

}

DBCTable BlizzardDatabase::ReadTable(std::string tableName)
{
    auto absoluteFilePath = _databaseFilesLocation.append("\\").append(tableName);
    char buffer[4];
    std::ifstream fileStream;
    fileStream.open(absoluteFilePath, std::ifstream::binary);
    fileStream.read(buffer,4);

    std::string str(buffer,4);

    std::cout << "File Header:" << str << std::endl;

    if (str == "WDC3")
    {
        auto reader = WDC3Reader(fileStream);
    }

    fileStream.close();

    return DBCTable();
}