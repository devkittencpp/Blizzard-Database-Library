#pragma once
#include<string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "DbcTable.h"
#include "WDC3Reader.h"
#include <DatabaseDefinition.h>
#include <Shared.h>

class BlizzardDatabase
{
private:
	std::string _databaseFilesLocation;
	std::string _databaseDefinitionFilesLocation;
public:
	BlizzardDatabase(std::string databaseCollectionDirectory, std::string databaseDefinitionDirectory);

	DBCTable ReadTable(std::string tableName);
};