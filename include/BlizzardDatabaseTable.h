#pragma once
#include<vector>
#include "BlizzardDatabaseRecord.h"

class BlizzardDatabaseTable
{
private:
    std::vector<BlizzardDatabaseRecord> _records;
public:
    BlizzardDatabaseTable() = default;
};

