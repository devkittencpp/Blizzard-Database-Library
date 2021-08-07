#include <DatabaseBuilder.h>

DatabaseBuilder::DatabaseBuilder(DBDefinition& fileDefinitions, Build& build): _definitions(fileDefinitions), _build(build)
{
	
}

void DatabaseBuilder::ConstructTable(std::fstream& fileStream, std::string& tableName)
{
	auto columnDefinitions = _definitions.columnDefinitions;
	auto tableVersionDefintions = _definitions.versionDefinitions;

	auto versionFound = false;
	VersionDefinitions versionDefinition;
	for (auto& versionDefiniton : tableVersionDefintions)
	{
		auto builds = versionDefiniton.builds;
		if (std::find(builds.begin(), builds.end(), _build) != builds.end())
		{
			versionDefinition = versionDefiniton;
			versionFound = true;
			break;
		}

		auto buildRanges = versionDefiniton.buildRanges;
		for (auto& buildRange : buildRanges)
		{
			if (buildRange.Contains(_build))
			{
				versionDefinition = versionDefiniton;
				versionFound = true;
				break;
			}
		}
	}

	if (versionFound == false)
	{
		std::cout << "Verion Not found" << std::endl;
		return;
	}
	
	std::cout << "Writing Sql Table" << std::endl;

	auto stringWriter = std::stringstream();

	std::cout << "---Table contents---" << std::endl;
	stringWriter << "---Table contents---" << std::endl;
	for (auto& definition : versionDefinition.definitions)
	{
		auto columnDefinition = columnDefinitions.at(definition.name);
		std::cout << "column: <" << columnDefinition.type << "> " << definition.name << std::endl;
		stringWriter << "--column: <" << columnDefinition.type << "> " << definition.name << std::endl;
	}

	std::cout << "---Foreign Keys---" << std::endl;
	stringWriter << "---Foreign Keys---" << std::endl;
	for (auto& definition : versionDefinition.definitions)
	{
		auto columnDefinition = columnDefinitions.at(definition.name);
		if (columnDefinition.hasForeignKey)
		{
			std::cout << "column: <" << columnDefinition.type << "> " << definition.name << " => " << columnDefinition.foreignTable << "[" << columnDefinition.foreignColumn << "]" << std::endl;
			stringWriter << "--column: <" << columnDefinition.type << "> " << definition.name << " => " << columnDefinition.foreignTable << "[" << columnDefinition.foreignColumn << "]" << std::endl;
		}
	}

	stringWriter << std::endl;
	stringWriter << "CREATE TABLE " << tableName << "(" << std::endl;
	auto index = 0;
	for (auto& definition : versionDefinition.definitions)
	{
		auto columnDefinition = columnDefinitions.at(definition.name);
		auto columnType = std::string();

		if (StringExtenstions::Compare(columnDefinition.type, "int"))
			columnType = "INTEGER";
		if (StringExtenstions::Compare(columnDefinition.type, "float"))
			columnType = "REAL";
		if (StringExtenstions::Compare(columnDefinition.type, "locstring"))
			columnType = "TEXT";
		if (StringExtenstions::Compare(columnDefinition.type, "string"))
			columnType = "TEXT";

		if (definition.isID)
		{
			stringWriter << std::tab <<"\"" << definition.name << "\"" << " " << columnType << " PRIMARY KEY";
		}
		else
		{
			stringWriter << std::tab << "\"" << definition.name << "\"" << " " << columnType;
		}

		if(index < versionDefinition.definitions.size() -1)
			stringWriter << "," << std::endl;

		index++;
	}

	index = 0;
	for (auto& definition : versionDefinition.definitions)
	{
		auto columnDefinition = columnDefinitions.at(definition.name);
		auto columnType = std::string();

		if (columnDefinition.hasForeignKey)
		{
			stringWriter << "," << std::endl;
			stringWriter << std::tab << "FOREIGN KEY(" << "\"" << definition.name << "\"" << ") REFERENCES  " << columnDefinition.foreignTable << "(" << "\"" << columnDefinition.foreignColumn << "\"" << ")";
		}

		index++;
	}
	stringWriter << ");" << std::endl << std::endl;

	fileStream << stringWriter.str();
}