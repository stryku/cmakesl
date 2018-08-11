#include "exec/execution_context.hpp"

namespace cmsl
{
    namespace exec
    {
        void execution_context::add_variable(cmsl::string_view name, int value)
        {
            current_scope->add_variable(name, value);
        }

        int& execution_context::get_variable(cmsl::string_view name)
        {
            return current_scope->get_variable(name);
        }

        bool execution_context::variable_exists(cmsl::string_view name) const
        {
            return current_scope->variable_exists(name);
        }

        void execution_context::enter_scope()
        {
            m_scopes.push(scope_context{ current_scope });
            current_scope = &m_scopes.top();
        }

        void execution_context::leave_scope()
        {
            m_scopes.pop();
            current_scope = &m_scopes.top();
        }
    }
}
