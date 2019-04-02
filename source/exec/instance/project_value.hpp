#pragma once

#include "common/string.hpp"

#include <functional>

namespace cmsl
{
    namespace facade
    {
        class cmake_facade;
    }

    namespace exec
    {
        namespace inst
        {
            class list_value;

            class project_value
            {
            public:
                explicit project_value(const std::string& name);

                std::string name() const;

                void add_executable(facade::cmake_facade& cmake_facade, const std::string& name, const list_value& sources);
                void add_library(facade::cmake_facade& cmake_facade, const std::string& name, const list_value& sources);

            private:
                std::vector<std::string> list_to_sources(facade::cmake_facade& cmake_facade, const list_value& sources);

            private:
                std::string m_name;
            };
        }
    }
}
