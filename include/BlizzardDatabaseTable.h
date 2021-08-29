#pragma once
#include <vector>
#include "BlizzardDatabaseRecord.h"
#include <structures/FileStructures.h>
#include <string>
namespace BlizzardDatabaseLib {

    class BlizzardDatabaseTable
    {
    private:
        std::vector<Structures::BlizzardDatabaseRow> _records;
    public:
        BlizzardDatabaseTable(std::vector<Structures::BlizzardDatabaseRow> records) : _records(records)
        {
        }
    };
}

