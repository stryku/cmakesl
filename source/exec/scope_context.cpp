#include "exec/scope_context.hpp"

#include "common/algorithm.hpp"

namespace cmsl
{
    namespace exec
    {
        scope_context::scope_context(const scope_context* parent)
            : m_parent{ parent }
        {}

        void scope_context::add_variable(cmsl::string_view name, int value)
        {
            m_variables[name] = value;
        }

        int* scope_context::get_variable(cmsl::string_view name)
        {
            const auto it = m_variables.find(name);
            return it != m_variables.cend() ? &it->second : nullptr;
        }

        bool scope_context::variable_exists(cmsl::string_view name) const
        {
            return cmsl::contains(m_variables, name);
        }
    }
}
