#include "exec/execution_context.hpp"

namespace cmsl
{
    namespace exec
    {
        void execution_context::add_variable(cmsl::string_view name, std::unique_ptr<instance> inst)
        {
            current_scope()->add_variable(name, std::move(inst));
        }

        instance* execution_context::get_variable(cmsl::string_view name)
        {
            return current_scope()->get_variable(name);
        }

        bool execution_context::variable_exists(cmsl::string_view name) const
        {
            return current_scope()->variable_exists(name);
        }

        void execution_context::enter_scope()
        {
            m_scopes.push(scope_context{ current_scope() });
        }

        void execution_context::leave_scope()
        {
            m_scopes.pop();
        }

        scope_context* execution_context::current_scope()
        {
            return m_scopes.empty() ? nullptr : &m_scopes.top();
        }

        const scope_context* execution_context::current_scope() const
        {
            return m_scopes.empty() ? nullptr : &m_scopes.top();
        }
    }
}
