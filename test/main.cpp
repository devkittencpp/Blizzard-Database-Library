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
        std::cout << "Expected Argument -DBP:<Path>" << std::endl;
    }

    if (dbdFileDirectory.empty())
    {
        std::cout << "No Defintion Directory Found" << std::endl;
        std::cout << "Expected Argument -DDP:<Path>" << std::endl;
    }

    const auto& build = BlizzardDatabaseLib::Structures::Build("9.1.0.39584");
    const auto& table = std::string("map");

    auto blizzardDatabase = BlizzardDatabaseLib::BlizzardDatabase(dbcFileDirectory, dbdFileDirectory);
    auto mapTable = blizzardDatabase.LoadTable(table, build);

    blizzardDatabase.UnloadTable(table);

    //auto achievementTable = blizzardDatabase.LoadTable("achievement");
    //auto areapoiTable = blizzardDatabase.LoadTable("areapoi");
    //auto BattlePetEffectPropertiesTable = blizzardDatabase.LoadTable("BattlePetEffectProperties");
    //auto CharacterLoadoutTable = blizzardDatabase.LoadTable("CharacterLoadout");
    //auto ItemSparseTable = blizzardDatabase.LoadTable("ItemSparse");
    //auto UIDungeonScoreRarityTable = blizzardDatabase.LoadTable("UIDungeonScoreRarity");

    return 0;
}