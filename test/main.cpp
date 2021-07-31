
#include<BlizzardDatabase.h>
#include<iostream>
#include<string>
#include<DbcTable.h>

int main(int argc, char* argv[])
{
    std::string dbcFileDirectory = std::string();

    for(auto index =0; index < argc; index++)
    {
        auto argument = std::string(argv[index]);
        auto path = std::string("-P");
        auto match = strncmp(argument.c_str(), path.c_str(), path.size());

        if(match == 0) //match found
        {
            dbcFileDirectory =  argument.substr(3, argument.size() -3);
            std::cout << "Path Found:" << dbcFileDirectory << std::endl;
        }
    }

    if(dbcFileDirectory.empty())
    {
        std::cout << "No Directory Found" << std::endl;
        std::cout << "Expected Argument -P <Path>" << std::endl;
    }
    
    auto blizzardDatabase = BlizzardDatabase(dbcFileDirectory);

    auto table = blizzardDatabase.ReadTable("map.db2");

    return 0;
}