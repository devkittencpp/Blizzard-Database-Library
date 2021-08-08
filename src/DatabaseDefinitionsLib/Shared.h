#pragma once
#include <vector>
#include <string>
#include <sstream>

namespace std {
    template <typename _CharT, typename _Traits>
    inline basic_ostream<_CharT, _Traits>&
        tab(basic_ostream<_CharT, _Traits>& __os) {
        return __os.put(__os.widen('\t'));
    }
}

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
        auto result = strcmp(lhs.c_str(), rhs.c_str());

        if (result == 0)
            return true;
        return false;
    }
};
