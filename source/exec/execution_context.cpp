#include "exec/execution_context.hpp"

#include "common/assert.hpp"
#include "exec/instance/instance_reference.hpp"

namespace cmsl::exec
{
        void execution_context::add_variable(cmsl::string_view name, std::unique_ptr<instance_t> inst)
        {
            current_scope().add_variable(name, std::move(inst));
        }

        execution_context::instance_t* execution_context::get_variable(cmsl::string_view name)
        {
            return current_scope().get_variable(name);
        }

        bool execution_context::variable_exists(cmsl::string_view name) const
        {
            return current_scope().variable_exists(name);
        }

        void execution_context::enter_scope()
        {
            auto current = m_scopes.empty() ? nullptr : &current_scope();
            m_scopes.push(scope_context{ current });
        }

        void execution_context::leave_scope()
        {
            m_scopes.pop();
        }

        scope_context& execution_context::current_scope()
        {
            // Todo: introduce CMSL_UNREACHABLE_IF(condition, message)
            if(m_scopes.empty())
            {
                CMSL_UNREACHABLE("No scope");
            }
            return m_scopes.top();
        }

        const scope_context& execution_context::current_scope() const
        {
            if(m_scopes.empty())
            {
                // Todo: introduce CMSL_UNREACHABLE_IF(condition, message)
                CMSL_UNREACHABLE("No scope");
            }
            return m_scopes.top();
        }

        void execution_context::enter_member_function_scope(instance_t* class_instance)
        {
            m_scopes.push(scope_context{ class_instance });
            add_variable("this", std::make_unique<inst::instance_reference>(*class_instance));
        }

        execution_context::instance_t *execution_context::get_this()
        {
            return get_variable("this");
        }
}
