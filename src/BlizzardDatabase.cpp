#include "BlizzardDatabase.h"

namespace BlizzardDatabaseLib
{
    BlizzardDatabase::BlizzardDatabase(const std::string& databaseCollectionDirectory, const std::string& databaseDefinitionDirectory) :
        _databaseFilesLocation(databaseCollectionDirectory), _databaseDefinitionFilesLocation(databaseDefinitionDirectory)
    {

    }

    BlizzardDatabaseTable BlizzardDatabase::ReadTable(const std::string& tableName, const Structures::Build& build)
    {
        auto absoluteFilePathOfDatabaseTable = _databaseFilesLocation + "\\" + tableName + ".db2";
        auto absoluteFilePathOfDatabaseTableDefinition = _databaseDefinitionFilesLocation + "\\" + tableName + ".dbd";

        auto databaseDefinition = DatabaseDefinition(absoluteFilePathOfDatabaseTableDefinition);
        auto tableDefinition = Structures::VersionDefinition();
        auto tableFound = databaseDefinition.For(build, tableDefinition);

        if (tableFound == false)
            std::cout << "Verion Not found" << std::endl;

        std::ifstream fileStream;
        fileStream.open(absoluteFilePathOfDatabaseTable, std::ifstream::binary);

        auto streamReader = Stream::StreamReader(fileStream);
        auto fileFormatIdentifier = streamReader.ReadString(4);

        if (!fileStream.is_open())
            std::cout << "FileClosed?" << std::endl;

        std::cout << "File Header Format: " << fileFormatIdentifier << std::endl;

        std::vector<Structures::BlizzardDatabaseRow> rows;
        if (Extension::String::Compare(fileFormatIdentifier, std::string("WDC3")))
        {
            auto reader = WDC3Reader(streamReader);
            rows = reader.ReadRows(tableDefinition);
        }

        fileStream.close();

        return BlizzardDatabaseTable();
    }
}
