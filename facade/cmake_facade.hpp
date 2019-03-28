#pragma once

#include <tuple>
#include <string>
#include <vector>

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

            virtual std::string get_current_binary_dir() const = 0;
            virtual std::string get_current_source_dir() const = 0;

            virtual void add_executable(const std::string& name, const std::vector<std::string>& sources) = 0; // Todo: Change sources to vector of string_views
            virtual void add_library(const std::string& name, const std::vector<std::string>& sources) = 0; // Todo: Change sources to vector of string_views

            virtual void target_link_library(const std::string& target_name, const std::string& library_name) = 0; // Todo: Change sources to vector of string_views
        };
    }
}
