#pragma once
#include <vector>
#include <structures/FileStructures.h>

namespace BlizzardDatabaseLib {
    namespace Reader {
        struct IBlizzardTableReader
        {
            virtual void LoadTableStructure() = 0;
            virtual Structures::BlizzardDatabaseRow RecordById(unsigned int Id) = 0;
            virtual std::vector<Structures::BlizzardDatabaseRow> Records() = 0; //This will be replaced by an iterator
            virtual std::size_t RecordCount() = 0;
        };
    }
}

