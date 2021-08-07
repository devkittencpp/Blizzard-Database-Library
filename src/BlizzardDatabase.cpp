#include "BlizzardDatabase.h"

BlizzardDatabase::BlizzardDatabase(std::string databaseCollectionDirectory, std::string databaseDefinitionDirectory, std::string databaseSqlDirectory) :
    _databaseFilesLocation(databaseCollectionDirectory), _databaseDefinitionFilesLocation(databaseDefinitionDirectory), _databaseSqlDirectory(databaseSqlDirectory)
{

}

DBCTable BlizzardDatabase::ReadTable(std::string tableName)
{
    auto build = Build("9.1.0.38549");

    auto absoluteFilePathOfDatabaseTable = _databaseFilesLocation + "\\" + tableName +".db2";
    auto absoluteFilePathOfDatabaseTableDefinition = _databaseDefinitionFilesLocation + "\\" + tableName+".dbd";
    auto absoluteFilePathOfSqlGeneration = _databaseSqlDirectory + "\\sql\\" + tableName+".sql";

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

    std::fstream sqlFileStream;
    sqlFileStream.open(absoluteFilePathOfSqlGeneration, std::ifstream::out);
    if (sqlFileStream.is_open())
        std::cout <<"[OPENED] =>" << absoluteFilePathOfSqlGeneration  << std::endl;

    tableBuilder.ConstructTable(sqlFileStream, tableName);

    sqlFileStream.close();
    fileStream.close();

    return DBCTable();
}

void BlizzardDatabase::CreateDatabase()
{
    auto build = Build("9.1.0.38549");
    auto absoluteFilePathOfSqlDatabaseGeneration = _databaseSqlDirectory + "\\sql\\database.sql";

    std::fstream databaseFile;
    databaseFile.open(absoluteFilePathOfSqlDatabaseGeneration, std::ifstream::out | std::ifstream::in);

    for (const auto& entry : std::filesystem::directory_iterator(_databaseDefinitionFilesLocation))
    {
        std::cout << entry.path().filename().replace_extension("").generic_string() << std::endl;

        if (!entry.path().has_extension())
            continue;

        auto fileName = entry.path().filename().replace_extension("").generic_string();
        auto absoluteFilePathOfDatabaseTable = _databaseFilesLocation + "\\" + fileName + ".db2";
        auto absoluteFilePathOfDatabaseTableDefinition = _databaseDefinitionFilesLocation + "\\" + fileName + ".dbd";

        auto databaseDefinition = DatabaseDefinition(absoluteFilePathOfDatabaseTableDefinition);
        auto tableDefinition = databaseDefinition.Read();

        auto tableBuilder = DatabaseBuilder(tableDefinition, build);

        std::ifstream fileStream;
        fileStream.open(absoluteFilePathOfDatabaseTable, std::ifstream::binary);

        auto streamReader = StreamReader(fileStream);
        auto fileFormatIdentifier = streamReader.ReadString(4);

        std::cout << "File Header:" << fileFormatIdentifier << std::endl;
      
        tableBuilder.ConstructTable(databaseFile, fileName);

        fileStream.close();
    }

    databaseFile.close();
}