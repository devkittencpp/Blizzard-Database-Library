#pragma once
#include <string>
#include <map>
#include <vector>

class Build
{
    //Populate later
};

class BuildRange
{
    //Populate later
};

struct ColumnDefinition
{
    std::string type;
    std::string foreignTable;
    std::string foreignColumn;
    bool verified;
    std::string comment;
};

struct Definition
{
    int size;
    int arrLength;
    std::string name;
    bool isID;
    bool isRelation;
    bool isNonInline;
    bool isSigned;
    std::string comment;
};

struct DBDefinition
{
    std::map<std::string, ColumnDefinition> columnDefinitions;
    std::vector<VersionDefinitions> versionDefinitions;
};

struct VersionDefinitions
{
    std::vector<Build> builds;
    std::vector<BuildRange> buildRanges;
    std::vector<std::string> layoutHashes;
    std::string comment;
    std::vector<Definition> definitions;
};