#pragma once
#include <sstream>
#include <FileStructures.h>

class DatabaseEngine
{
public:
	virtual void GenerateSql(std::stringstream& stream, VersionDefinitions& defintion, ColumnDefinition& columnDefinitions) = 0;
};