#pragma once

#include <cstddef>
#include <string>

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

            virtual ~cmake_facade() = default;

            virtual version get_cmake_version() const = 0;
            virtual void fatal_error(const std::string& what) const = 0;
        };
    }
}
