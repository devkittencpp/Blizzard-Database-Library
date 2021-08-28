#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <structures/FileStructures.h>
#include <map>
#include <vector>
#include <algorithm>
#include <regex>
#include <extensions/StringExtensions.h>

namespace BlizzardDatabaseLib
{
	class DatabaseDefinition
	{
	private:
		std::string _databaseDefinitionFile;
	public:
		DatabaseDefinition(std::string databaseDefinitionsDirectory);
		Structures::DBDefinition Read();
		bool For(Structures::Build& build, Structures::VersionDefinition& definition);
	};
}
