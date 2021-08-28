#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <BlizzardDatabaseTable.h>
#include <readers/WDC3Reader.h>
#include <DatabaseDefinition.h>
#include <extensions/StringExtensions.h>
#include <sstream>

namespace BlizzardDatabaseLib
{
	class BlizzardDatabase
	{
	private:
		const std::string _databaseFilesLocation;
		const std::string _databaseDefinitionFilesLocation;
	public:
		BlizzardDatabase(const std::string& databaseCollectionDirectory, const std::string& databaseDefinitionDirectory);

		BlizzardDatabaseTable ReadTable(const std::string& tableName, const Structures::Build& build);
	};
}