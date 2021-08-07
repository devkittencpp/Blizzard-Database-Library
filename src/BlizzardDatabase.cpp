#include "BlizzardDatabase.h"

BlizzardDatabase::BlizzardDatabase(std::string databaseCollectionDirectory, std::string databaseDefinitionDirectory, std::string databaseSqlDirectory) :
    _databaseFilesLocation(databaseCollectionDirectory), _databaseDefinitionFilesLocation(databaseDefinitionDirectory), _databaseSqlDirectory(databaseSqlDirectory)
{

}

DBCTable BlizzardDatabase::ReadTable(std::string tableName)
{
    auto build = Build("9.1.0.38549");

    auto absoluteFilePathOfDatabaseTable = _databaseFilesLocation.append("\\").append(tableName).append(".db2");
    auto absoluteFilePathOfDatabaseTableDefinition = _databaseDefinitionFilesLocation.append("\\").append(tableName).append(".dbd");
    auto absoluteFilePathOfSqlGeneration = _databaseSqlDirectory.append("\\").append(tableName).append(".sql");

    auto databaseDefinition = DatabaseDefinition(absoluteFilePathOfDatabaseTableDefinition);
    auto tableDefinition = databaseDefinition.Read();

    auto tableBuilder = DatabaseBuilder(tableDefinition, build);

    std::ifstream fileStream;
    fileStream.open(absoluteFilePathOfDatabaseTable, std::ifstream::binary);

    auto streamReader = StreamReader(fileStream);
    auto fileFormatIdentifier = streamReader.ReadString(4);

    std::cout << "File Header:" << fileFormatIdentifier << std::endl;

    if (fileFormatIdentifier == "WDC3")
    {
        auto reader = WDC3Reader(fileStream);
    }

    std::ofstream sqlFileStream;
    sqlFileStream.open(absoluteFilePathOfSqlGeneration, std::ifstream::out);
    if (sqlFileStream.is_open())
        std::cout <<"[OPENED] =>" << absoluteFilePathOfSqlGeneration  << std::endl;

    tableBuilder.ConstructTable(sqlFileStream, tableName);

    sqlFileStream.close();
    fileStream.close();

    return DBCTable();
}