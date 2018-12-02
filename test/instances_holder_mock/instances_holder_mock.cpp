#include "exec/instance/instances_holder.hpp"
#include "instances_holder_mock/instances_holder_mock.hpp"

namespace
{
    cmsl::exec::inst::test::instances_holder_mock* instances_holder_mock_ptr;
}

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            namespace test
            {
                instances_holder_mock::instances_holder_mock()
                {
                    assert(!instances_holder_mock_ptr);
                    instances_holder_mock_ptr = this;
                }

                instances_holder_mock::~instances_holder_mock()
                {
                    instances_holder_mock_ptr = nullptr;
                }
            }

            instances_holder::instances_holder(context_provider& e)
                : m_ctx_provider{ e }
            {}

            instance* instances_holder::create(instance_value_t value)
            {
                return instances_holder_mock_ptr->create(std::move(value));
            }

            instance* instances_holder::create(const ast::type& type, instance_members_t members)
            {
                return instances_holder_mock_ptr->create(type, std::move(members));
            }

            instance* instances_holder::create(const ast::type& t)
            {
                return instances_holder_mock_ptr->create(t);
            }

            instance* instances_holder::create_void()
            {
                return instances_holder_mock_ptr->create_void();
            }

            void instances_holder::store(std::unique_ptr<instance> i)
            {
                instances_holder_mock_ptr->store(std::move(i));
            }

            std::unique_ptr<instance> instances_holder::gather_ownership(inst::instance* instance_ptr)
            {
                return instances_holder_mock_ptr->gather_ownership(instance_ptr);
            }
        }
    }
}
