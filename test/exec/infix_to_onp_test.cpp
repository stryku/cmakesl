#include <gmock/gmock.h>

#include "ast/builtin_ast_context.hpp"
#include "exec/onp/infix_to_onp.hpp"
#include "lexer/token/token.hpp"

#include "test/common/tokens.hpp"

namespace cmsl
{
    namespace test
    {
        namespace exec
        {
            namespace infix_to_onp
            {
                using tokens_container_t = cmsl::lexer::token::token_container_t;
                using inf_to_onp_t = cmsl::exec::onp::infix_to_onp;

                using namespace cmsl::test::common;

                struct InfixToOnpState
                {
                    tokens_container_t in;
                    tokens_container_t expected_result;
                };

                using CorrectInput = testing::TestWithParam<InfixToOnpState>;

                TEST_P(CorrectInput, GetCorrectOutput)
                {
                    const auto& state = GetParam();
                    cmsl::ast::builtin_ast_context ctx;
                    const auto result = inf_to_onp_t{ state.in, ctx }.convert();
                    ASSERT_THAT(result, state.expected_result);
                }

                const auto values = testing::Values(
                    InfixToOnpState{
                        { token_integer("4") }, // 4
                        { token_integer("4") } // 4
                    },
                    InfixToOnpState{
                            { token_integer("4"), token_plus(), token_integer("5") }, // 4 + 5
                            { token_integer("4"), token_integer("5"), token_plus() } // 4 5 +
                    },
                    InfixToOnpState{
                            { token_integer("4"), token_plus(), token_integer("5"), token_plus(), token_integer("6") }, // 4 + 5 + 6
                            { token_integer("4"), token_integer("5"), token_plus(), token_integer("6"), token_plus() } // 4 5 + 6 +
                    },
                    InfixToOnpState{
                            { token_integer("4"), token_plus(), token_identifier("foo"), token_dot(), token_identifier("bar") }, // 4 + foo.bar
                            { token_integer("4"), token_identifier("foo"), token_identifier("bar"), token_dot(), token_plus() } // 4 foo bar . +
                    },
                    InfixToOnpState{
                            { token_integer("4"), token_plus(), token_identifier("foo"), token_dot(), token_identifier("bar"), token_dot(), token_identifier("baz") }, // 4 + foo.bar.baz
                            { token_integer("4"), token_identifier("foo"), token_identifier("bar"), token_dot(), token_identifier("baz"), token_dot(), token_plus() } // 4 foo bar . baz . +
                    }
                );

                INSTANTIATE_TEST_CASE_P(InfixToOnp, CorrectInput, values);
            }
        }
    }
}