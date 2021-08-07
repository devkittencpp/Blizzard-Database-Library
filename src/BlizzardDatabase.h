#pragma once
#include<string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "DbcTable.h"
#include "WDC3Reader.h"
#include <DatabaseDefinition.h>
#include <DatabaseBuilder.h>
#include <Shared.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <sstream>

class BlizzardDatabase
{
private:
	std::string _databaseFilesLocation;
	std::string _databaseSqlDirectory;
	std::string _databaseDefinitionFilesLocation;
public:
	BlizzardDatabase(std::string databaseCollectionDirectory, std::string databaseDefinitionDirectory, std::string databaseSqlDirectory);

	DBCTable ReadTable(std::string tableName);
	void CreateDatabase();
};