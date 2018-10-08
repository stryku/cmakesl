#include "ast/builtin_ast_context.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace ast
    {
        namespace test
        {
            namespace project_type
            {
                using testing::NotNull;
                using testing::Eq;

                class BuiltinAstContext : public testing::Test
                {
                protected:
                    builtin_ast_context m_ctx;
                };

                TEST_F(BuiltinAstContext, HasProjectType)
                {
                    const auto project_type = m_ctx.find_type("project");
                    ASSERT_THAT(project_type, NotNull());
                }

                TEST_F(BuiltinAstContext, ProjectType_ConstructorSignature)
                {
                    const auto project_type = m_ctx.find_type("project");
                    const auto ctor = project_type->get_function("project");
                    ASSERT_THAT(ctor, NotNull());

                    const auto params_declaration = ctor->get_params_declarations();

                    EXPECT_THAT(params_declaration.size(), Eq(1u));

                    const auto string_type = m_ctx.find_type("string");
                    EXPECT_THAT(params_declaration[0].param_type, Eq(string_type));
                }

                TEST_F(BuiltinAstContext, ProjectType_AddExecutableSignature)
                {
                    const auto project_type = m_ctx.find_type("project");
                    const auto add_executable = project_type->get_function("add_executable");
                    ASSERT_THAT(add_executable, NotNull());

                    const auto params_declaration = add_executable->get_params_declarations();

                    EXPECT_THAT(params_declaration.size(), Eq(1u));

                    const auto source_list_type = m_ctx.find_type("list<string>");
                    EXPECT_THAT(params_declaration[0].param_type, Eq(source_list_type));
                }
            }
        }
    }
}
