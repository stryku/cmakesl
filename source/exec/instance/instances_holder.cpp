#include "instances_holder.hpp"
#include "unnamed_instance.hpp"
#include "instance_factory.hpp"
#include "exec/exec.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            instances_holder::instances_holder(executor &e)
                : m_exec{e}
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
                return instance_factory{m_exec.get_ast_ctx(),
                                        m_exec.get_exec_ctx()};
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
