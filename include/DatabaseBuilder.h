#pragma once
#include <iostream>
#include <StreamReader.h>
#include <FileStructures.h>
#include <exception>
#include <algorithm>
#include <FileStructures.h>
#include <sstream>
#include <fstream>

class DatabaseBuilder
{
private:
	Build& _build;
	DBDefinition& _definitions;
public:
	DatabaseBuilder(DBDefinition& fileDefinitions, Build& build);
	void ConstructTable(std::fstream& fileStream, std::string& tableName, std::vector<BlizzardDatabaseRow> rows);
};