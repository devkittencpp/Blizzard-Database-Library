#include "BlizzardDatabase.h"

BlizzardDatabase::BlizzardDatabase(std::string databaseCollectionDirectory) : _databaseFilesLocation(databaseCollectionDirectory)
{

}

DBCTable BlizzardDatabase::ReadTable(std::string tableName)
{
    auto absoluteFilePath = _databaseFilesLocation.append("\\").append(tableName);
    char buffer[4];
    std::ifstream fileStream;
    fileStream.open(absoluteFilePath);
    fileStream.read(buffer,4);
    fileStream.close();

    std::string str(buffer,4);

    std::cout << "File Header:" << str << std::endl;

    return DBCTable();
}