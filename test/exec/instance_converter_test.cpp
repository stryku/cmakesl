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
            using instance_value_t = cmsl::exec::inst::instance_value_t;
            using int_t = cmsl::exec::inst::int_t;

            namespace convert_to_type
            {
                struct ConvertToType_FundamentalArithmetic_State
                {
                    instance_value_t value_to_convert;
                    instance_value_t expected_bool_value;
                    instance_value_t expected_int_value;
                    instance_value_t expected_double_value;
                };

                struct ConvertToType : public testing::TestWithParam<ConvertToType_FundamentalArithmetic_State>
                {};

                TEST_P(ConvertToType, FundamentalArithmetic_GetCorrectValue)
                {
                    test_context_provider ctx_provider;
                    instances_holder_t instances{ctx_provider};
                    instance_converter_t converter{instances};

                    const auto& state = GetParam();

                    auto instance_to_convert = instances.create(state.value_to_convert);

                    // to bool
                    {
                        const auto desired_type = ctx_provider.get_ast_ctx().find_type("bool");
                        auto converted_instance = converter.convert_to_type(instance_to_convert, *desired_type);
                        auto converted_value = converted_instance->get_value();
                        ASSERT_THAT(converted_value, state.expected_bool_value);
                    }

                    // to int
                    {
                        const auto desired_type = ctx_provider.get_ast_ctx().find_type("int");
                        auto converted_instance = converter.convert_to_type(instance_to_convert, *desired_type);
                        auto converted_value = converted_instance->get_value();
                        ASSERT_THAT(converted_value, state.expected_int_value);
                    }

                    // to double
                    {
                        const auto desired_type = ctx_provider.get_ast_ctx().find_type("double");
                        auto converted_instance = converter.convert_to_type(instance_to_convert, *desired_type);
                        auto converted_value = converted_instance->get_value();
                        ASSERT_THAT(converted_value, state.expected_double_value);
                    }
                }

                const auto values = testing::Values(
                        ConvertToType_FundamentalArithmetic_State{ true, true, int_t{ 1 }, 1.0 },
                        ConvertToType_FundamentalArithmetic_State{ false, false, int_t{ 0 }, 0.0 },
                        ConvertToType_FundamentalArithmetic_State{ 0.9, true, int_t{ 0 }, 0.9 },
                        ConvertToType_FundamentalArithmetic_State{ 1.9, true, int_t{ 1 }, 1.9 },
                        ConvertToType_FundamentalArithmetic_State{ 2.9, true, int_t{ 2 }, 2.9 }
                );

                INSTANTIATE_TEST_CASE_P(InstanceConverter, ConvertToType, values);
            }
        }
    }
}
