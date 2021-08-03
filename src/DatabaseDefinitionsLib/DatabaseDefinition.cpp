#include <DatabaseDefinition.h>

DatabaseDefinition::DatabaseDefinition(std::string databaseDefinitionsFile) : _databaseDefinitionFile(databaseDefinitionsFile)
{

}


void DatabaseDefinition::Read()
{
    std::ifstream fileStream;
    fileStream.open(_databaseDefinitionFile);

    auto validTypes = std::vector<std::string>{ "uint", "int", "float", "string", "locstring" };
    auto columnDefinitionDictionary = std::map<std::string, ColumnDefinition>();
    auto fileLines = std::vector<std::string>();
    auto line = std::string();

    while (std::getline(fileStream, line))
    {
        fileLines.push_back(line);
    }

    auto lineCounter = 1;
    auto totalLines = fileLines.size();
    auto firstLine = fileLines[0];

    if (firstLine.starts_with("COLUMNS") == false)
    {
        std::cout << "unexpected format" << std::endl;
        return;
    }

    while (lineCounter < totalLines)
    {
        auto line = fileLines[lineCounter];

        if (line.empty())
            break;

        auto columnDefinition = ColumnDefinition();

        if (line.find_first_not_of(" ") == std::string::npos)
        {
            std::cout << "format invalid, no type specified" << std::endl;
            return;
        }

        auto indexOfCommentsStart = line.find_first_of("//");
        auto indexOfForeignKeyStart = line.find_first_of("<");
        auto indexOfForeignKeyEnd = line.find_first_of(">");
        auto indexOfNameStart = line.find_first_of(" ");
        auto indexOfNameEnd = line.find(" ", indexOfNameStart + 1);
        auto indexofTypeEnd = indexOfForeignKeyStart == std::string::npos ? indexOfNameStart : indexOfForeignKeyStart;
        auto columnType = line.substr(0, indexofTypeEnd);

        if (!std::any_of(validTypes.begin(), validTypes.end(), [columnType](std::string comparison) {return comparison == columnType; }))
        {
            std::cout << "column type invalid : " << columnType << std::endl;
            return;
        }

        columnDefinition.type = columnType;

        if (indexOfForeignKeyStart != std::string::npos && indexOfForeignKeyEnd != std::string::npos)
        {         
            auto startIndex = indexOfForeignKeyStart + 1;
            auto foreignKey = line.substr(startIndex, indexOfForeignKeyEnd - startIndex);
            auto foreignKeyComponents = Split(foreignKey, '::');

            if (foreignKeyComponents.size() != 2)
            {
                std::cout << "Foreign Key Malformed" << std::endl;
                return;
            }

            columnDefinition.foreignTable = foreignKeyComponents[0];
            columnDefinition.foreignColumn = foreignKeyComponents[1];
        }

        auto name = std::string("Nameless Column > ").append(std::to_string(lineCounter));
    
        if (indexOfNameEnd == std::string::npos)
        {
            name = line.substr(indexOfNameStart+1);
        }

        if (indexOfNameStart != std::string::npos && indexOfNameEnd != std::string::npos)
        {
            auto adjustedIndexOfNameStart = indexOfNameStart + 1;
            name = line.substr(adjustedIndexOfNameStart, indexOfNameEnd - adjustedIndexOfNameStart);
        }

        if (name.ends_with("?"))
        {
            name = name.substr(0, name.size() - 1);
            columnDefinition.verified = true;
        }

        if (indexOfCommentsStart != std::string::npos)
        {
            columnDefinition.comment = line.substr(indexOfCommentsStart, line.size() - indexOfCommentsStart);
        }

        columnDefinitionDictionary.emplace(name, columnDefinition);

        lineCounter++;
    }

    auto x = 0;
}

std::vector<std::string> DatabaseDefinition::Split(const std::string & s, char delim)
{
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim)) 
    {
        if(!item.empty())
            result.push_back(item);
    }

    return result;
}
