#include "ast/type.hpp"
#include "ast/parser.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token/token.hpp"

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
            using tokens_container_t = cmsl::lexer::token::token_container_t;
            using parser_t = cmsl::ast::parser;

            namespace get_type
            {
                namespace builtin
                {
                    using BuiltinTypeToken = testing::TestWithParam<tokens_container_t>;

                    TEST_P(BuiltinTypeToken, GetBuiltInType)
                    {
                        const auto tokens = GetParam();
                        auto p = parser_t{ tokens.cbegin() };
                        const auto type = p.get_type();
                        ASSERT_THAT(type.is_builtin(), true);
                    }

                    const auto values = testing::Values(
                        tokens_container_t{ token_t{ token_type_t::t_int } },
                        tokens_container_t{ token_t{ token_type_t::t_real } }
                    );

                    INSTANTIATE_TEST_CASE_P(Parser_GetType, BuiltinTypeToken, values);
                }

                namespace identifier
                {
                    using IdentifierToken = testing::TestWithParam<tokens_container_t>;

                    TEST_P(IdentifierToken, GetNotBuiltinType)
                    {
                        const auto tokens = GetParam();
                        auto p = parser_t{ tokens.cbegin() };
                        const auto type = p.get_type();
                        ASSERT_THAT(type.is_builtin(), false);
                    }

                    const auto values = testing::Values(
                        tokens_container_t{ token_t{ token_type_t::identifier } }
                    );

                    INSTANTIATE_TEST_CASE_P(Parser_GetType, IdentifierToken, values);
                }
            }

            namespace get_parameters_declaration
            {
                TEST(Parser_GetParametersDeclaration, Empty_GetEmpty)
                {
                    const auto tokens = tokens_container_t{
                        token_t{ token_type_t::open_paren },
                        token_t{ token_type_t::close_paren }
                    };

                    auto p = parser_t{ tokens.cbegin() };
                    const auto parameters = p.get_parameters_declaration();
                    ASSERT_THAT(parameters.empty(), true);
                }
            }
        }
    }
}
