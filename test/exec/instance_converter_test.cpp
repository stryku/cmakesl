#include "exec/instance/instance_converter.hpp"
#include "exec/instance/instances_holder.hpp"

#include "test/exec/test_context_provider.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace test
    {
        namespace instance_converter
        {
            using instance_converter_t = cmsl::exec::inst::instance_converter;
            using instances_holder_t = cmsl::exec::inst::instances_holder;
            using instance_value_t = cmsl::exec::inst::instance_value_t ;

            TEST(InstanceConverter_ConvertToType, FundamentalArithmetic_GetCorrectValue)
            {
                // INTRODUCE AST AND EXEC CONTEXT PROVIDER INTERFACE AND USE IT INSTEAD OF EXEC

                test_context_provider ctx_provider;
                instances_holder_t instances{ ctx_provider };
                instance_converter_t converter{ instances };

                {
                    instance_value_t val = true;
                    auto instance = instances.create(val);
                    const auto desired_type = ctx_provider.get_ast_ctx().find_type("int");
                    auto converted = converter.convert_to_type(instance, *desired_type);
                    auto converted_value = boost::get<int>(converted->get_value());
                    ASSERT_THAT(converted_value, static_cast<int>(true));
                }
            }
        }
    }
}
