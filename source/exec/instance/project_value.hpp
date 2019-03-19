#pragma once

#include "common/string.hpp"

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
                explicit project_value(facade::cmake_facade& cmake_facade,
                                       cmsl::string_view name);

                std::string name() const;

                void add_executable(const std::string& name, const list_value& sources);

            private:
                facade::cmake_facade& m_cmake_facade;
                cmsl::string_view m_name;
            };
        }
    }
}
