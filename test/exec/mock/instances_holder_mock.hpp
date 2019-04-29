#pragma once

#include "exec/instance/instance_value_variant.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instances_holder_interface.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::inst
{
            class instance;

            namespace test
            {
                struct instances_holder_mock : public instances_holder_interface
                {
                    MOCK_METHOD1(store, void(std::unique_ptr<instance>));
                    MOCK_METHOD1(gather_ownership, std::unique_ptr<instance>(inst::instance*));

                    MOCK_METHOD1(create, instance *(instance_value_variant));

                    MOCK_METHOD1(create_reference, instance *(instance
                            &));
                    MOCK_METHOD1(create, instance*(const sema::sema_type&));
                    MOCK_METHOD2(create, instance*(const sema::sema_type&, instance_value_variant));

                    MOCK_METHOD0(create_void, instance*());
                };
            }
}
