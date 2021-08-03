#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <FileStructures.h>
#include <map>
#include <vector>
#include <algorithm>
#include <regex>
#include<Shared.h>

class DatabaseDefinition
{
private:
	std::string _databaseDefinitionFile;
public:
	DatabaseDefinition(std::string databaseDefinitionsDirectory);
	DBDefinition Read();
};