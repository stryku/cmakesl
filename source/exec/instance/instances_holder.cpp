#include "instances_holder.hpp"
#include "unnamed_instance.hpp"
#include "instance_factory.hpp"
#include "exec/context_provider.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            instances_holder::instances_holder(context_provider &e)
                : m_ctx_provider{ e }
            {}

            inst::instance *instances_holder::create(instance_value_t value)
            {
                auto inst = get_factory().create(value);
                auto ptr = inst.get();
                m_instances.emplace_back(std::move(inst));
                return ptr;
            }

            instance_factory instances_holder::get_factory()
            {
                return instance_factory{ m_ctx_provider.get_ast_ctx(),
                                         m_ctx_provider.get_exec_ctx() };
            }

            inst::instance *instances_holder::create(const ast::type& t)
            {
                auto inst = get_factory().create(t);;
                auto ptr = inst.get();
                m_instances.emplace_back(std::move(inst));
                return ptr;
            }
        }
    }
}
