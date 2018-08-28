#include "exec/scope_context.hpp"

#include "common/algorithm.hpp"

namespace cmsl
{
    namespace exec
    {
        scope_context::scope_context(scope_context* parent)
            : m_parent{ parent }
            , m_class_instance{ nullptr }
        {}

        scope_context::scope_context(instance_t* class_instance)
            : m_parent{ nullptr }
            , m_class_instance{ class_instance }
        {}

        void scope_context::add_variable(cmsl::string_view name, std::unique_ptr<inst::instance> inst)
        {
            m_variables[name] = std::move(inst);
        }

        inst::instance* scope_context::get_variable(cmsl::string_view name)
        {
            const auto it = m_variables.find(name);
            if(it != m_variables.cend())
            {
                return it->second.get();
            }
            else if(m_parent != nullptr)
            {
                return m_parent->get_variable(name);
            }
            else if(m_class_instance != nullptr)
            {
                return m_class_instance->get_member(name);
            }

            return nullptr;
        }

        const inst::instance* scope_context::get_variable(cmsl::string_view name) const
        {
            const auto it = m_variables.find(name);
            if(it != m_variables.cend())
            {
                return it->second.get();
            }
            else if(m_parent != nullptr)
            {
                return m_parent->get_variable(name);
            }

            return nullptr;
        }

        bool scope_context::variable_exists(cmsl::string_view name) const
        {
            return get_variable(name) != nullptr;
        }
    }
}
