#include "ast/ast_context.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace test
    {
        namespace ast_context
        {
            namespace add_find_type
            {
                TEST(AstContext, Empty_FindType_GetNull)
                {
                    ast::ast_context ctx;
                    const auto t = ctx.find_type("int");
                    ASSERT_THAT(t, nullptr);
                }
            }
        }
    }
}
