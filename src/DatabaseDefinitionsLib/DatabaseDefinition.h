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

class DatabaseDefinition
{
private:
	std::string _databaseDefinitionFile;
public:
	DatabaseDefinition(std::string databaseDefinitionsDirectory);

	//Tempt until I understand the format of DBD files
	void Read();
    std::vector<std::string> Split(const std::string& s, char delim);
};