#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <BlizzardDatabaseTable.h>
#include <WDC3Reader.h>
#include <DatabaseDefinition.h>
#include <DatabaseBuilder.h>
#include <Extensions.h>
#include <sstream>

namespace BlizzardDatabaseLib
{
	class BlizzardDatabase
	{
	private:
		std::string _databaseFilesLocation;
		std::string _databaseDefinitionFilesLocation;
	public:
		BlizzardDatabase(std::string databaseCollectionDirectory, std::string databaseDefinitionDirectory);

		BlizzardDatabaseTable ReadTable(std::string tableName);
		void CreateDatabase();
	};
}