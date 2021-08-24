#pragma once
#include<vector>
#include "DbcRecord.h"

class DBCTable
{
private:
    std::vector<DBCRecord> _records;
public:
    DBCTable() = default;
};

