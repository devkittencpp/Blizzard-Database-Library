#pragma once
#include <StreamReader.h>
#include <string>
#include <map>
#include <future>
#include <Types.h>
#include <Shared.h>

class WDC3Row
{
private:
	int _id;
	const BitReader& _reader;
	const std::map<long, std::string>& _stringLookup;
public:
	WDC3Row(int Id, const BitReader& reader,  const std::map<long, std::string>& stringLookup);

	void ReadFieldValue(std::string& columnType, const FieldMeta& fieldMeta, const ColumnMetaData& columnMeta, const std::vector<Int32>& palletData, const std::map<int, Int32>& commonData);
};