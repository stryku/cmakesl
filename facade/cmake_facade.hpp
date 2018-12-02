#pragma once

#include <cstddef>
#include <string>
#include <tuple>
#include <memory>
#include <string>

class cmMakefile;

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

                bool operator==(const version& other) const
                {
                    return std::tie(major, minor, patch, tweak) == std::tie(other.major, other.minor, other.patch, other.tweak);
                }

                bool operator<(const version& other) const
                {
                    const auto lhs = {major, minor, patch, tweak};
                    const auto rhs = {other.major, other.minor, other.patch, other.tweak};

                    return std::lexicographical_compare(std::cbegin(lhs), std::cend(lhs),
                                                        std::cbegin(rhs), std::cend(rhs));
                }

                bool operator<=(const version& other) const
                {
                    return *this < other || *this == other;
                }
            };

            virtual ~cmake_facade() = default;

            virtual version get_cmake_version() const = 0;
            virtual void fatal_error(const std::string& what) const = 0;
            virtual void register_project(const std::string& name) = 0;

        private:
            std::unique_ptr<cmMakefile> m_makefile;
        };
    }
}
