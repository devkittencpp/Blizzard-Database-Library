#pragma once
#include <string>
#include <map>
#include <vector>
#include <Shared.h>

class Build
{
private:
    short _expansion;
    short _major;
    short _minor;
    unsigned int build;
public:
    Build(std::string buildString)
    {
        auto buildVersionElements = StringExtenstions::Split(buildString,'.');

        _expansion = std::atoi(buildVersionElements[0].c_str());
        _major = std::atoi(buildVersionElements[1].c_str());
        _minor = std::atoi(buildVersionElements[2].c_str());
        build = std::atoi(buildVersionElements[3].c_str());
    }

};

class BuildRange
{
private:
    Build _minBuild;
    Build _maxBuild;
public:
    BuildRange(Build minBuild, Build maxBuild) : _minBuild(minBuild), _maxBuild(_maxBuild) {}
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

