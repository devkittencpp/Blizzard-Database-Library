#pragma once
#include <string>

struct ColumnDefinition
{
    std::string type;
    std::string foreignTable;
    std::string foreignColumn;
    bool verified;
    std::string comment;
};