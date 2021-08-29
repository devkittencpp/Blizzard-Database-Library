#pragma once
#include <string>

namespace BlizzardDatabaseLib {
    namespace Extension {

        class Memory
        {
        public:
            bool static IsEmpty(char* data, size_t length)
            {
                return memcmp(data, data + 1, length - 1) == 0;
            }
        };
    }
}