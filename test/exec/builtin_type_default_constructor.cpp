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

                    TEST_F(BuiltinTypeDefaultValue, Bool_GetFalse)
                    {
                        const auto val = get_value_of_type("bool");
                        const auto bool_value = boost::get<bool>(val);
                        ASSERT_THAT(bool_value, Eq(false));
                    }

                    TEST_F(BuiltinTypeDefaultValue, Int_GetZero)
                    {
                        using int_t = cmsl::exec::inst::int_t;
                        const auto val = get_value_of_type("int");
                        const auto int_value = boost::get<int_t>(val);
                        ASSERT_THAT(int_value, Eq(int_t{0}));
                    }

                    TEST_F(BuiltinTypeDefaultValue, Double_GetZero)
                    {
                        const auto val = get_value_of_type("double");
                        const auto double_value = boost::get<double>(val);
                        ASSERT_THAT(double_value, Eq(0.0));
                    }

                    TEST_F(BuiltinTypeDefaultValue, String_GetEmpty)
                    {
                        const auto val = get_value_of_type("string");
                        const auto string_value = boost::get<std::string>(val);
                        ASSERT_THAT(string_value, Eq(""));
                    }

                    TEST_F(BuiltinTypeDefaultValue, Version_GetZeros)
                    {
                        //todo
                        //using version_t = cmsl::exec::inst::version;
                        //const auto val = get_value_of_type("version");
                        //const auto version_value = boost::get<version_t>(val);
                        //ASSERT_THAT(version_value, Eq(version_t{ 0, 0, 0, 0 }));
                    }
                }
            }
        }
    }
}
