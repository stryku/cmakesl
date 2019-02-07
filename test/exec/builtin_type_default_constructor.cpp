#include "ast/builtin_ast_context.hpp"
#include "exec/instance/instance_factory.hpp"
#include "exec/instance/instance.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            namespace test
            {
                namespace default_constructor
                {
                    using testing::Eq;

                    class BuiltinTypeDefaultValue : public ::testing::Test
                    {
                    protected:
                        instance_value_t get_value_of_type(cmsl::string_view type_name)
                        {
                            ast::builtin_ast_context ctx;
                            const auto &type = *ctx.find_type(type_name);
                            const auto instance = instance_factory{}.create(type);
                            return instance->get_value();
                        }
                    };

                    TEST_F(BuiltinTypeDefaultValue, DISABLED_Bool_GetFalse)
                    {
                        const auto val = get_value_of_type("bool");
                        const auto bool_value = val.get_bool();
                        ASSERT_THAT(bool_value, Eq(false));
                    }

                    TEST_F(BuiltinTypeDefaultValue, DISABLED_Int_GetZero)
                    {
                        using int_t = cmsl::exec::inst::int_t;
                        const auto val = get_value_of_type("int");
                        const auto int_value = val.get_int();
                        ASSERT_THAT(int_value, Eq(int_t{0}));
                    }

                    TEST_F(BuiltinTypeDefaultValue, DISABLED_Double_GetZero)
                    {
                        const auto val = get_value_of_type("double");
                        const auto double_value = val.get_double();
                        ASSERT_THAT(double_value, Eq(0.0));
                    }

                    TEST_F(BuiltinTypeDefaultValue, DISABLED_String_GetEmpty)
                    {
                        const auto val = get_value_of_type("string");
                        const auto string_value = val.get_string_cref();
                        ASSERT_THAT(string_value, Eq(""));
                    }
                }
            }
        }
    }
}
