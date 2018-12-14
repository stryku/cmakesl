#include "ast/builtin_ast_context.hpp"
#include "ast/class_type.hpp"

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

                // Todo: move to a common file
                class BuiltinAstContext : public testing::Test
                {
                protected:
                    builtin_ast_context m_ctx;
                };

                TEST_F(BuiltinAstContext, HasVersionClassType)
                {
                    const auto version_type = m_ctx.find_type("version");
                    ASSERT_THAT(version_type, NotNull());

                    const auto version_class_type = dynamic_cast<const class_type*>(version_type);
                    ASSERT_THAT(version_class_type, NotNull());
                }

                TEST_F(BuiltinAstContext, VersionType_ConstructorSignature)
                {
                    const auto version_type = m_ctx.find_type("version");
                    const auto ctor = version_type->get_function("version");
                    ASSERT_THAT(ctor, NotNull());

                    const auto params_declaration = ctor->get_params_declarations();

                    EXPECT_THAT(params_declaration.size(), Eq(4u)); // major, minor, patch and tweak

                    const auto int_type = m_ctx.find_type("int");
                    EXPECT_THAT(params_declaration[0].param_type, Eq(int_type));
                    EXPECT_THAT(params_declaration[1].param_type, Eq(int_type));
                    EXPECT_THAT(params_declaration[2].param_type, Eq(int_type));
                    EXPECT_THAT(params_declaration[3].param_type, Eq(int_type));
                }

                TEST_F(BuiltinAstContext, VersionType_HasMembers)
                {
                    const auto version_type = dynamic_cast<const class_type*>(m_ctx.find_type("version"));
                    const auto& members_declaration = version_type->get_members_decl();

                    ASSERT_THAT(members_declaration.size(), Eq(4u));

                    const auto int_type = m_ctx.find_type("int");
                    EXPECT_THAT(members_declaration[0].name, Eq("major"));
                    EXPECT_THAT(members_declaration[0].ty, Eq(int_type));
                    EXPECT_THAT(members_declaration[1].name, Eq("minor"));
                    EXPECT_THAT(members_declaration[1].ty, Eq(int_type));
                    EXPECT_THAT(members_declaration[2].name, Eq("patch"));
                    EXPECT_THAT(members_declaration[2].ty, Eq(int_type));
                    EXPECT_THAT(members_declaration[3].name, Eq("tweak"));
                    EXPECT_THAT(members_declaration[3].ty, Eq(int_type));
                }
            }
        }
    }
}
