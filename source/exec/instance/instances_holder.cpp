#include "instances_holder.hpp"
#include "instance_factory.hpp"
#include "exec/instance/instance.hpp"
#include "exec/context_provider.hpp"

#include "common/assert.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            instances_holder::instances_holder(const sema::sema_context_interface& sema_ctx)
                : m_sema_ctx{ &sema_ctx }
            {}

            instances_holder::instances_holder(context_provider &e)
                : m_ctx_provider{ &e }
            {}

            std::unique_ptr<instance> instances_holder::gather_ownership(inst::instance *instance_ptr)
            {
                auto found = std::find_if(std::begin(m_instances), std::end(m_instances),
                                         [instance_ptr](const auto& i)
                                         {
                                             return i.get() == instance_ptr;
                                         });

                if(found == std::end(m_instances))
                {
                    CMSL_UNREACHABLE("Gathering instance that doesn't belong to this instances holder instance");
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

            inst::instance *instances_holder::create2(instance_value_t value)
            {
                auto instance = instance_factory2{}.create(std::move(value), *m_sema_ctx);
                auto ptr = instance.get();
                m_instances.emplace_back(std::move(instance));
                return ptr;
            }

            inst::instance *instances_holder::create2_reference(inst::instance &referenced_instance)
            {
                auto instance = instance_factory2{}.create_reference(referenced_instance);
                auto ptr = instance.get();
                m_instances.emplace_back(std::move(instance));
                return ptr;
            }

            inst::instance *instances_holder::create2(const sema::sema_type &type)
            {
                auto instance = instance_factory2{}.create(type);
                auto ptr = instance.get();
                m_instances.emplace_back(std::move(instance));
                return ptr;
            }

            inst::instance *instances_holder::create2_void()
            {
                return create2(true); // Todo: introduce void types
            }

            inst::instance *instances_holder::create2(const sema::sema_type &type, instance_value_t value)
            {
                auto instance = instance_factory2{}.create(type, std::move(value));
                auto ptr = instance.get();
                m_instances.emplace_back(std::move(instance));
                return ptr;
            }
        }
    }
}
