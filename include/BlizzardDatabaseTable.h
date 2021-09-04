#pragma once
#include <vector>
#include <BlizzardDatabaseRecord.h>
#include <readers/IBlizzardTableReader.h>
#include <structures/FileStructures.h>
#include <string>
#include <algorithm>

namespace BlizzardDatabaseLib {

    class BlizzardDatabaseRecordCollection
    {
    private:
        int _minIndex = 0;
        int _maxIndex = 0;
        int _currentIndex = -1;
        std::shared_ptr<Reader::IBlizzardTableReader> _tableReader;
        Structures::BlizzardDatabaseRow _currentRecord;
    public:
        BlizzardDatabaseRecordCollection(std::shared_ptr<Reader::IBlizzardTableReader> tableReader)
        {
            _maxIndex = tableReader->RecordCount();
            _tableReader = tableReader;
        }

        bool HasRecords()
        {
            return _currentIndex < _maxIndex - 1;
        }

        Structures::BlizzardDatabaseRow& Next()
        {
           _currentIndex++;
           _currentRecord = _tableReader->Record(_currentIndex);      
           return _currentRecord;
        }

        Structures::BlizzardDatabaseRow& First()
        {
            _currentRecord = _tableReader->Record(_minIndex);
            return _currentRecord;
        }

        Structures::BlizzardDatabaseRow& Last()
        {
            _currentRecord = _tableReader->Record(_maxIndex);
            return _currentRecord;
        }
    };


    class BlizzardDatabaseTable
    {
        friend class BlizzardDatabase;
    private:
        std::shared_ptr<Reader::IBlizzardTableReader> _tableReader;
    public:
        BlizzardDatabaseTable(std::shared_ptr<Reader::IBlizzardTableReader> tableReader) : _tableReader(tableReader)
        {

        }

        ~BlizzardDatabaseTable()
        {
            _tableReader->CloseAllSections();
        }

        unsigned int RecordCount()
        {
            return _tableReader->RecordCount();
        }

        Structures::BlizzardDatabaseRow Record(unsigned int id)
        {
           return _tableReader->RecordById(id);
        }

        BlizzardDatabaseRecordCollection Records()
        {
            return BlizzardDatabaseRecordCollection(_tableReader);
        }

        Structures::BlizzardDatabaseRow GetRecordDefinition()
        {
            return _tableReader->RecordDefinition();
        }

        void WriterRecord(Structures::BlizzardDatabaseRow& newRecord)
        {

        }
    private:
        void LoadTableStructure()
        {  
            _tableReader->LoadTableStructure();
        }
    };
}

