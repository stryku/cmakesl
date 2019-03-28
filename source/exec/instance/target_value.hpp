#pragma once

#include <string>

namespace cmsl
{
    namespace facade
    {
        class cmake_facade;
    }

    namespace exec::inst
    {
        class target_value
        {
        public:
            explicit target_value(std::string name);

            std::string name() const;

            void link_to(facade::cmake_facade &cmake_facade, const target_value &target) const;

        private:
            std::string m_name;
        };
    }
}
