#include "ast/builtin_ast_context.hpp"

#include <gmock/gmock.h>

namespace cmsl
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
                ast::builtin_ast_context m_ctx;
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
        }
    }
}
