#include "exec/instance/target_value.hpp"

#include "cmake_facade.hpp"

namespace cmsl::exec::inst
{
    target_value::target_value(std::string name)
        : m_name{ std::move(name) }
    {}

    std::string target_value::name() const
    {
        return m_name;
    }

    void target_value::link_to(facade::cmake_facade &cmake_facade, const target_value &target) const
    {
        cmake_facade.target_link_library(m_name, target.name());
    }
}