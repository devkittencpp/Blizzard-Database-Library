#pragma once
#include<string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "DbcTable.h"

class BlizzardDatabase
{
private:
	std::string _databaseFilesLocation;
public:
	BlizzardDatabase(std::string databaseCollectionDirectory);

	DBCTable ReadTable(std::string tableName);
};