#include <BlizzardDatabase.h>
#include <iostream>
#include <string>
#include <BlizzardDatabaseTable.h>
#include <filesystem>

int main(int argc, char* argv[])
{
    std::string dbcFileDirectory = std::string();
    std::string dbdFileDirectory = std::string();

    for(auto index =0; index < argc; index++)
    {
        auto argument = std::string(argv[index]);
        auto path = std::string("-DBP");
        auto match = strncmp(argument.c_str(), path.c_str(), path.size());

        if(match == 0) //match found
        {
            dbcFileDirectory =  argument.substr(5, argument.size() - 5);
            std::cout << "Database Directory Found: " << dbcFileDirectory << std::endl;
        }

        argument = std::string(argv[index]);
        path = std::string("-DDP");
        match = strncmp(argument.c_str(), path.c_str(), path.size());

        if (match == 0) //match found
        {
            dbdFileDirectory = argument.substr(5, argument.size() - 5);
            std::cout << "Database Definitions Directory Found: " << dbdFileDirectory << std::endl;
        }
    }

    if(dbcFileDirectory.empty())
    {
        std::cout << "No Database Directory Found" << std::endl;
        std::cout << "Expected Argument -DBP <Path>" << std::endl;
    }

    if (dbdFileDirectory.empty())
    {
        std::cout << "No Defintion Directory Found" << std::endl;
        std::cout << "Expected Argument -DDP <Path>" << std::endl;
    }
    
    auto blizzardDatabase = BlizzardDatabase(dbcFileDirectory, dbdFileDirectory, dbdFileDirectory);
    auto mapTable = blizzardDatabase.ReadTable("map");
    //auto achievementTable = blizzardDatabase.ReadTable("achievement");
    //auto areapoiTable = blizzardDatabase.ReadTable("areapoi");
    //auto BattlePetEffectPropertiesTable = blizzardDatabase.ReadTable("BattlePetEffectProperties");
    //auto CharacterLoadoutTable = blizzardDatabase.ReadTable("CharacterLoadout");
    //auto ItemSparseTable = blizzardDatabase.ReadTable("ItemSparse");
    //auto ItemSparseTable = blizzardDatabase.ReadTable("UIDungeonScoreRarity");

    return 0;
}