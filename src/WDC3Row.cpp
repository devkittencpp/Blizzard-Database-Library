#include <WDC3Row.h>

WDC3Row::WDC3Row(int Id, const BitReader& reader,  const std::map<long, std::string>& stringLookup): 
	_id(Id),_reader(reader),  _stringLookup(stringLookup)
{
}

void WDC3Row::ReadFieldValue(std::string& columnType, const FieldMeta& fieldMeta, const ColumnMetaData& columnMeta, 
	const std::vector<Int32>& palletData, const std::map<int, Int32>& commonData)
{
   
}
