#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <BlizzardDatabaseTable.h>
#include <readers/BlizzardTableReaderFactory.h>
#include <DatabaseDefinition.h>
#include <extensions/StringExtensions.h>
#include <sstream>
#include <functional>

namespace BlizzardDatabaseLib
{
	class BlizzardDatabase
	{
		friend class BlizzardDatabaseTable;
	private:
		const std::string _databaseDefinitionFilesLocation;
		const Structures::Build _build;
		Reader::BlizzardTableReaderFactory _blizzardTableReaderFactory;

		std::map<std::string, std::shared_ptr<BlizzardDatabaseTable>> _loadedTables;

    std::function<std::ifstream(std::string const&)> _file_read_callback;

	public:
		BlizzardDatabase(std::function<std::ifstream(std::string const&)> file_read_callback,
                     const std::string& databaseDefinitionDirectory, const Structures::Build& build);

		const BlizzardDatabaseTable& LoadTable(const std::string& tableName);
		void UnloadTable(const std::string& tableName);
	};
}