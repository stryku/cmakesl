#include "lexer/lexer.hpp"
#include "ast/parser.hpp"

#include <gmock/gmock.h>

#include <sstream>

namespace cmsl
{
    namespace test
    {
        namespace parser
        {
            using lexer_t = cmsl::lexer::lexer;
            using token_t = cmsl::lexer::token::token;
            using token_type_t = cmsl::lexer::token::token_type;
            using parser_t = cmsl::ast::parser;

            namespace get_type
            {
                using BuilinTypeState = std::vector<token_t>;

                struct BuiltinType : public testing::TestWithParam<BuilinTypeState>
                {};

                TEST_P(BuiltinType, GetBuiltInType)
                {
                    const auto tokens = GetParam();
                    auto p = parser_t{ tokens.cbegin() };
                    const auto type = p.get_type();
                    ASSERT_THAT(type.is_builtin(), true);
                }

                const auto values = testing::Values(
                    BuilinTypeState{ token_t{ token_type_t::t_int } },
                    BuilinTypeState{ token_t{ token_type_t::t_real } }
                );

                INSTANTIATE_TEST_CASE_P(Parser, BuiltinType, values)
            }
        }
    }
}
