#pragma once
#include <vector>
#include <BlizzardDatabaseRecord.h>
#include <readers/IBlizzardTableReader.h>
#include <structures/FileStructures.h>
#include <string>

namespace BlizzardDatabaseLib {

    class BlizzardDatabaseTable
    {
        friend class BlizzardDatabase;
    private:
        std::shared_ptr<Reader::IBlizzardTableReader> _tableReader;
    public:
        BlizzardDatabaseTable(std::shared_ptr<Reader::IBlizzardTableReader> tableReader) : _tableReader(tableReader)
        {

        }

        void Record(unsigned int id)
        {
            auto records = _tableReader->RecordById(id);
        }

        void Records()
        {
            _tableReader->Records();
        }
    private:
        void LoadTableStructure()
        {  
            _tableReader->LoadTableStructure();
        }
    };
}

