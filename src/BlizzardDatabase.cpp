#include "BlizzardDatabase.h"

BlizzardDatabase::BlizzardDatabase(std::string databaseCollectionDirectory, std::string databaseDefinitionDirectory, std::string databaseSqlDirectory) :
    _databaseFilesLocation(databaseCollectionDirectory), _databaseDefinitionFilesLocation(databaseDefinitionDirectory), _databaseSqlDirectory(databaseSqlDirectory)
{

}

DBCTable BlizzardDatabase::ReadTable(std::string tableName)
{
    auto build = Build("9.1.0.39584");

    auto absoluteFilePathOfDatabaseTable = _databaseFilesLocation + "\\" + tableName +".db2";
    auto absoluteFilePathOfDatabaseTableDefinition = _databaseDefinitionFilesLocation + "\\" + tableName+".dbd";
    auto absoluteFilePathOfSqlDatabaseGeneration = _databaseSqlDirectory + "\\sql\\" + tableName + ".sql";

    std::fstream databaseFile;
    databaseFile.open(absoluteFilePathOfSqlDatabaseGeneration, std::ifstream::out | std::ifstream::in | std::ifstream::trunc);

    auto databaseDefinition = DatabaseDefinition(absoluteFilePathOfDatabaseTableDefinition);
    auto tableDefinitions = databaseDefinition.Read();
    auto tableDefinition = VersionDefinition();
    auto tableFound = databaseDefinition.For(build, tableDefinition);

    if(tableFound == false)
        std::cout << "Verion Not found" << std::endl;

    auto tableBuilder = DatabaseBuilder(tableDefinitions, build);

    std::ifstream fileStream;
    fileStream.open(absoluteFilePathOfDatabaseTable, std::ifstream::binary);

    auto streamReader = StreamReader(fileStream);
    auto fileFormatIdentifier = streamReader.ReadString(4);

    if(!fileStream.is_open())
        std::cout << "FileClosed?" << std::endl;

    std::cout << "File Header Format: " << fileFormatIdentifier << std::endl;

    std::vector<BlizzardDatabaseRow> rows;
    if(StringExtenstions::Compare(fileFormatIdentifier,std::string("WDC3")))
    {
        auto reader = WDC3Reader(streamReader);
        rows = reader.ReadRows(tableDefinition);
    }
    tableBuilder.ConstructTable(databaseFile, tableName, rows);

    fileStream.close();

    return DBCTable();
}

void BlizzardDatabase::CreateDatabase()
{
    auto build = Build("9.1.0.39584");
   

    for (const auto& entry : std::filesystem::directory_iterator(_databaseDefinitionFilesLocation))
    {
        std::cout << entry.path().filename().replace_extension("").generic_string() << std::endl;

        if (!entry.path().has_extension())
            continue;

        auto fileName = entry.path().filename().replace_extension("").generic_string();
        auto absoluteFilePathOfSqlDatabaseGeneration = _databaseSqlDirectory + "\\sql\\" + fileName + ".sql";

        std::fstream databaseFile;
        databaseFile.open(absoluteFilePathOfSqlDatabaseGeneration, std::ifstream::out | std::ifstream::in | std::ifstream::trunc);

     
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
      
        auto tableVersionDefinition = VersionDefinition();
        auto tableFound = databaseDefinition.For(build, tableVersionDefinition);

        std::vector<BlizzardDatabaseRow> rows;
        if (tableFound == false)
            std::cout << "Verion Not found" << std::endl;
        else
        {        
            if (StringExtenstions::Compare(fileFormatIdentifier, std::string("WDC3")))
            {
                auto reader = WDC3Reader(streamReader);
                rows = reader.ReadRows(tableVersionDefinition);
            }
        }
        tableBuilder.ConstructTable(databaseFile, fileName, rows);

        fileStream.close();
        databaseFile.close();
    }
}