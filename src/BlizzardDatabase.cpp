#include "BlizzardDatabase.h"

BlizzardDatabase::BlizzardDatabase(std::string databaseCollectionDirectory, std::string databaseDefinitionDirectory) : _databaseFilesLocation(databaseCollectionDirectory), _databaseDefinitionFilesLocation(databaseDefinitionDirectory)
{

}

DBCTable BlizzardDatabase::ReadTable(std::string tableName)
{
    auto absoluteFilePathOfDatabaseTable = _databaseFilesLocation.append("\\").append(tableName).append(".db2");
    auto absoluteFilePathOfDatabaseTableDefinition = _databaseDefinitionFilesLocation.append("\\").append(tableName).append(".dbd");

    auto databaseDefinition = DatabaseDefinition(absoluteFilePathOfDatabaseTableDefinition);
    //auto tableDefinition = databaseDefinition.Read();
    databaseDefinition.Read();

    std::ifstream fileStream;
    fileStream.open(absoluteFilePathOfDatabaseTable, std::ifstream::binary);

    auto streamReader = StreamReader(fileStream);
    auto fileFormatIdentifier = streamReader.ReadString(4);

    std::cout << "File Header:" << fileFormatIdentifier << std::endl;

    if (fileFormatIdentifier == "WDC3")
    {
        auto reader = WDC3Reader(fileStream);
    }

    fileStream.close();

    return DBCTable();
}