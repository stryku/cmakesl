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

            std::unique_ptr<instance> instances_holder::gather_ownership(inst::instance *instance_ptr)
            {
                auto found = std::find_if(std::begin(m_instances), std::end(m_instances),
                                         [instance_ptr](const auto& i)
                                         {
                                             return i.get() == instance_ptr;
                                         });

                if(found == std::end(m_instances))
                {
                    return nullptr;
                }

                auto ret = std::move(*found);
                m_instances.erase(found);
                return std::move(ret);
            }

            void instances_holder::store(std::unique_ptr<instance> i)
            {
                m_instances.emplace_back(std::move(i));
            }
        }
    }
}
