#pragma once

#include <cstddef>

namespace cmsl
{
    namespace facade
    {
        class cmake_facade
        {
        public:
            struct version
            {
                size_t major;
                size_t minor;
                size_t patch;
                size_t tweak;
            };

            virtual ~cmake_facade();

            virtual version get_cmake_version() const = 0;
        };
    }
}
