#include "BlizzardDatabase.h"
#include <cassert>

namespace BlizzardDatabaseLib
{
    BlizzardDatabase::BlizzardDatabase(std::function<std::ifstream(std::string const&)> file_read_callback
                                       , const std::string& databaseDefinitionDirectory
                                       , const Structures::Build& build)
      :
      _databaseDefinitionFilesLocation(databaseDefinitionDirectory)
      , _file_read_callback(std::move(file_read_callback))
      , _build(build)
    {
        _loadedTables = std::map<std::string, std::shared_ptr<BlizzardDatabaseTable>>();
        _blizzardTableReaderFactory = Reader::BlizzardTableReaderFactory();
    }

    const BlizzardDatabaseTable& BlizzardDatabase::LoadTable(const std::string& tableName)
    {
        if (_loadedTables.contains(tableName))
            std::cout << "Table already loaded" << std::endl;

        auto absoluteFilePathOfDatabaseTableDefinition =  std::filesystem::path(_databaseDefinitionFilesLocation) /
            (tableName + ".dbd");

        auto databaseDefinition = DatabaseDefinition(absoluteFilePathOfDatabaseTableDefinition.generic_string());
        auto tableDefinition = Structures::VersionDefinition();
        auto tableFound = databaseDefinition.For(_build, tableDefinition);

        if (!tableFound)
            std::cout << "Verion Not found" << std::endl;

        auto fileStream = _file_read_callback(tableName);

        auto streamReader = std::make_shared<Stream::StreamReader>(fileStream);
        auto fileFormatIdentifier = streamReader->ReadString(4);

        auto tableReader = _blizzardTableReaderFactory.For(streamReader, tableDefinition,fileFormatIdentifier);

        auto constructedTable = std::make_shared<BlizzardDatabaseTable>(tableReader);
        constructedTable->LoadTableStructure();

        _loadedTables.emplace(tableName, constructedTable);

        return *_loadedTables[tableName];
    }

    void BlizzardDatabase::UnloadTable(const std::string& tableName)
    {
        if (!_loadedTables.contains(tableName))
            std::cout << "Table Not Loaded" << std::endl;

        auto table = _loadedTables.at(tableName);

        table.reset();

        _loadedTables.erase(tableName);
    }
}
