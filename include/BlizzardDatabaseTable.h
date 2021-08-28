#pragma once
#include<vector>
#include "BlizzardDatabaseRecord.h"

namespace BlizzardDatabaseLib {

    class BlizzardDatabaseTable
    {
    private:
        std::vector<BlizzardDatabaseRecord> _records;
    public:
        BlizzardDatabaseTable() = default;
    };
}

