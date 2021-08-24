#pragma once
#include <string>
#include <map>
#include <vector>
#include <Build.h>
#include <BuildRange.h>

struct BlizzardDatabaseColumn
{
    std::string Value;
    int ReferenceId;
};

struct BlizzardDatabaseRow
{
    std::map<std::string, BlizzardDatabaseColumn> Columns;
};

struct ColumnDefinition
{
    std::string type;
    std::string foreignTable;
    std::string foreignColumn;
    bool hasForeignKey;
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
    bool IsInline;
    bool isSigned;
    std::string comment;
};

struct VersionDefinitions
{
    std::vector<Build> builds;
    std::vector<BuildRange> buildRanges;
    std::vector<std::string> layoutHashes;
    std::string comment;
    std::vector<Definition> definitions;
};

struct DBDefinition
{
    std::map<std::string, ColumnDefinition> columnDefinitions;
    std::vector<VersionDefinitions> versionDefinitions;
};

struct VersionDefinition
{
    std::map<std::string, ColumnDefinition> columnDefinitions;
    VersionDefinitions versionDefinitions;
};


