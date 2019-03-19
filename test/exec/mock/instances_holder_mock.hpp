#pragma once

#include "exec/instance/instance_value.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instances_holder_interface.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance;

            namespace test
            {
                struct instances_holder_mock : public instances_holder_interface
                {
                    MOCK_METHOD1(create, instance*(instance_value_t));
                    MOCK_METHOD2(create, instance*(const ast::type&, instance_members_t));
                    MOCK_METHOD1(create, instance*(const ast::type&));
                    MOCK_METHOD0(create_void, instance*());
                    MOCK_METHOD1(store, void(std::unique_ptr<instance>));
                    MOCK_METHOD1(gather_ownership, std::unique_ptr<instance>(inst::instance*));

                    MOCK_METHOD1(create2, instance*(instance_value_t));
                    MOCK_METHOD1(create2_reference, instance*(instance&));
                    MOCK_METHOD1(create2, instance*(const sema::sema_type&));
                    MOCK_METHOD2(create2, instance*(const sema::sema_type&, instance_value_t));
                    MOCK_METHOD0(create2_void, instance*());
                };
            }
        }
    }
}
