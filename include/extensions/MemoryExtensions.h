#pragma once
#include <string>

namespace BlizzardDatabaseLib {
    namespace Extension {

        class Memory
        {
        public:
            bool static IsEmpty(char* data, size_t length)
            {
                if (length == 0) return 1;
                if (*data) return 0;
                return memcmp(data, data + 1, length - 1) == 0;
            }
        };
    }
}