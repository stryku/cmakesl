#include "ast/parser2.hpp"
#include "ast/ast_node.hpp"
#include "ast/block_node.hpp"

#include "errors/errors_observer.hpp"

#include "test/common/tokens.hpp"
#include "test/errors_observer_mock/errors_observer_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace ast
    {
        namespace test
        {
            using token_t = cmsl::lexer::token::token;
            using token_type_t = cmsl::lexer::token::token_type;
            using tokens_container_t = cmsl::lexer::token::token_container_t;
            using parser_t = cmsl::ast::parser2;

            using ::testing::_;
            using ::testing::IsNull;
            using ::testing::Values;
            using ::testing::TestWithParam;

            using namespace cmsl::test::common;

            struct errors_observer_and_mock
            {
                ::testing::StrictMock<errors::test::errors_observer_mock> mock;
                errors::errors_observer err_observer;
            };

            using errs_t = errors_observer_and_mock;

            namespace factor
            {
                using Factor = TestWithParam<token_t>;

                TEST_P(Factor, UnexpectedToken_ReportError)
                {
                    errs_t errs;
                    EXPECT_CALL(errs.mock, notify_error(_));

                    const auto tokens = tokens_container_t{ GetParam() };
                    parser2 p{ errs.err_observer, tokens };
                    auto result = p.factor();

                    EXPECT_THAT(result, IsNull());
                }

                const auto values = Values( token_undef(),
                                            token_dot(),
                                            token_open_brace(),
                                            token_close_brace(),
                                            token_open_square(),
                                            token_close_square(),
                                            token_equal(),
                                            token_equalequal(),
                                            token_minus(), // Todo: handle unary operators
                                            token_minusminus(), // Todo: handle unary operators
                                            token_minusequal(),
                                            token_plus(), // Todo: handle unary operators
                                            token_plusplus(), // Todo: handle unary operators
                                            token_plusequal(),
                                            token_amp(),
                                            token_ampamp(),
                                            token_ampequal(),
                                            token_pipe(),
                                            token_pipepipe(),
                                            token_pipeequal(),
                                            token_slash(),
                                            token_slashequal(),
                                            token_star(),
                                            token_starequal(),
                                            token_percent(),
                                            token_percentequal(),
                                            token_exclaim(),// Todo: handle unary operators
                                            token_exclaimequal(),
                                            token_xor(),
                                            token_xorequal(),
                                            token_semicolon(),
                                            token_comma(),
                                            token_less(),
                                            token_lessequal(),
                                            token_greater(),
                                            token_greaterequal(),
                                            token_kw_int(),
                                            token_kw_double(),
                                            token_kw_return(),
                                            token_kw_class(),
                                            token_kw_if(),
                                            token_kw_else(),
                                            token_kw_while(),
                                            token_kw_bool(),
                                            token_kw_string(),
                                            token_kw_list() );

                INSTANTIATE_TEST_CASE_P(Parser2ErrorsTest, Factor, values);
            }


            namespace binary_op
            {
                using BinaryOperator = TestWithParam<tokens_container_t>;
                TEST_P(BinaryOperator, MissingOperand_ReportError)
                {
                    errs_t errs;
                    EXPECT_CALL(errs.mock, notify_error(_));

                    const auto tokens = GetParam();
                    parser2 p{ errs.err_observer, tokens };
                    auto result = p.expr();

                    EXPECT_THAT(result, IsNull());
                }

                const auto id_token = token_identifier("foo");
                const auto op_token = token_star();
                const auto values = Values(
                        tokens_container_t{ id_token, op_token }, // foo *
                        tokens_container_t{ op_token, id_token }, // * foo
                        tokens_container_t{ id_token, op_token, token_semicolon() }, // foo * ;
                        tokens_container_t{ id_token, op_token, token_close_brace() }, // foo * }
                        tokens_container_t{ id_token, op_token, token_close_paren() }, // foo * )
                        tokens_container_t{ id_token, op_token, token_close_square() } // foo * ]
                );
                INSTANTIATE_TEST_CASE_P(Parser2ErrorsTest, BinaryOperator, values);
            }

            namespace function_call
            {
                using FunctionCall = TestWithParam<tokens_container_t>;
                TEST_P(FunctionCall, Malformed_ReportError)
                {
                    errs_t errs;
                    EXPECT_CALL(errs.mock, notify_error(_));

                    const auto tokens = GetParam();
                    parser2 p{ errs.err_observer, tokens };
                    auto result = p.expr();

                    EXPECT_THAT(result, IsNull());
                }

                // Todo: change all *_token to *_tok
                const auto id_token = token_identifier("foo");
                const auto param_token = token_identifier("bar");
                const auto op_token = token_open_paren();
                const auto cp_token = token_close_paren();
                const auto comma_token = token_comma();
                const auto values = Values(
                        tokens_container_t{ id_token, op_token }, // foo(
                        tokens_container_t{ id_token, op_token, token_close_brace() }, // foo(}
                        tokens_container_t{ id_token, op_token, token_close_square() }, // foo(]
                        tokens_container_t{ id_token, op_token, comma_token, cp_token }, // foo(,)
                        tokens_container_t{ id_token, op_token, param_token,  comma_token, cp_token }, // foo(bar,)
                        tokens_container_t{ id_token, op_token, comma_token, param_token, cp_token }, // foo(,bar)
                        tokens_container_t{ id_token, op_token, comma_token, cp_token } //foo(,)
                  );
                INSTANTIATE_TEST_CASE_P(Parser2ErrorsTest, FunctionCall, values);
            }

            namespace block
            {
                using Block = TestWithParam<tokens_container_t>;

                TEST_P(Block, Malformed_ReportError)
                {
                    errs_t errs;
                    EXPECT_CALL(errs.mock, notify_error(_));

                    const auto tokens = GetParam();
                    parser2 p{ errs.err_observer, tokens };
                    auto result = p.block();

                    EXPECT_THAT(result, IsNull());
                }

                const auto ob_token = token_open_brace();
                const auto cb_token = token_open_brace();

                const auto values = Values(
                        tokens_container_t{ ob_token }, // {
                        tokens_container_t{ cb_token }, // }
                        tokens_container_t{ ob_token, token_integer("1"), token_integer("2"), cb_token }, // Missing semicolon after expression { 1 2 }
                        tokens_container_t{ ob_token, token_integer("1"), token_integer("2"), cb_token }, // Missing semicolon after expression { 1 2 }
                        tokens_container_t{ ob_token, token_kw_return(), token_integer("1"), cb_token }, // Missing semicolon after expression { return 1 }
                        tokens_container_t{ ob_token, token_kw_return(), cb_token } // Missing semicolon after expression { return }
                );

                INSTANTIATE_TEST_CASE_P(Parser2ErrorTest, Block, values);
            }
        }
    }
}
