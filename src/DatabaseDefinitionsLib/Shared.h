#pragma once
#include <vector>
#include <string>
#include <sstream>

class StringExtenstions
{
public:
    static std::vector<std::string> Split(const std::string& s, char delim)
    {
        std::vector<std::string> result;
        std::stringstream ss(s);
        std::string item;

        while (std::getline(ss, item, delim))
        {
            if (!item.empty())
                result.push_back(item);
        }

        return result;
    }

    static bool Compare(const std::string& lhs, const std::string& rhs)
    {
        auto result = lhs.compare(rhs);

        if (result == 0)
            return true;
        return false;
    }
};
