#include "ast/builtin_ast_context.hpp"
#include "errors/errors_observer.hpp"
#include "errors_observer_mock/errors_observer_mock.hpp"
#include "ast/parser.hpp"
#include "ast/infix_node.hpp"
#include "ast/member_declaration.hpp"
#include "ast/if_else_node.hpp"
#include "ast/while_node.hpp"

#include "test/common/tokens.hpp"

#include <gmock/gmock.h>

#include <sstream>

using namespace testing;

namespace cmsl
{
    namespace test
    {
        namespace parser_error
        {
            using token_t = cmsl::lexer::token::token;
            using token_type_t = cmsl::lexer::token::token_type;
            using token_container_t = cmsl::lexer::token::token_container_t;
            using parser_t = cmsl::ast::parser;
            using builtin_ctx_t = ast::builtin_ast_context;

            using namespace cmsl::test::common;

            template <typename Tokens, typename Action>
            void report_error_test(Tokens&& tokens, Action&& action)
            {
                testing::StrictMock<errors_observer::errors_observer_mock> err_observer_mock;
                errors::errors_observer err_observer;

                EXPECT_CALL(err_observer_mock, notify_error(_)).Times(1);

                auto p = parser_t{ err_observer, tokens };
                action(p);
            }

            namespace get_type
            {
                namespace builtin_or_identifier
                {
                    using BuiltinOrIdentifier = testing::TestWithParam<token_container_t>;

                    TEST_P(BuiltinOrIdentifier, NoError)
                    {
                        testing::StrictMock<errors_observer::errors_observer_mock> err_observer_mock;
                        errors::errors_observer err_observer;
                        const auto t = token_kw_int;
                        const auto tokens = GetParam();
                        auto p = parser_t{ err_observer, tokens };
                        builtin_ctx_t ctx;
                        (void)p.get_type(ctx);
                    }

                    const auto values = testing::Values(
                        token_container_t{token_kw_int() },
                        token_container_t{token_kw_double() }
                    );

                    INSTANTIATE_TEST_CASE_P(ParserError_GetType, BuiltinOrIdentifier, values);
                }

                namespace unexpected_token
                {
                    using UnexpectedToken = testing::TestWithParam<token_container_t>;

                    TEST_P(UnexpectedToken, ReportError)
                    {
                        report_error_test(GetParam(), [](auto& parser)
                        {
                            builtin_ctx_t ctx;
                            (void)parser.get_type(ctx);
                        });
                    }

                    const auto values = testing::Values(
                        token_container_t{ token_integer() },
                        token_container_t{ token_double() },
                        token_container_t{ token_dot() },
                        token_container_t{ token_open_brace() },
                        token_container_t{ token_close_brace() },
                        token_container_t{ token_open_square() },
                        token_container_t{ token_close_square() },
                        token_container_t{ token_open_paren() },
                        token_container_t{ token_close_paren() },
                        token_container_t{ token_equal() },
                        token_container_t{ token_equalequal() },
                        token_container_t{ token_minus() },
                        token_container_t{ token_minusminus() },
                        token_container_t{ token_minusequal() },
                        token_container_t{ token_plus() },
                        token_container_t{ token_plusplus() },
                        token_container_t{ token_plusequal() },
                        token_container_t{ token_amp() },
                        token_container_t{ token_ampamp() },
                        token_container_t{ token_ampequal() },
                        token_container_t{ token_pipe() },
                        token_container_t{ token_pipepipe() },
                        token_container_t{ token_pipeequal() },
                        token_container_t{ token_slash() },
                        token_container_t{ token_slashequal() },
                        token_container_t{ token_star() },
                        token_container_t{ token_starequal() },
                        token_container_t{ token_percent() },
                        token_container_t{ token_percentequal() },
                        token_container_t{ token_exclaim() },
                        token_container_t{ token_exclaimequal() },
                        token_container_t{ token_xor() },
                        token_container_t{ token_xorequal() },
                        token_container_t{ token_string("") },
                        token_container_t{ token_semicolon() }
                    );

                    INSTANTIATE_TEST_CASE_P(ParserError_GetType, UnexpectedToken, values);
                }
            }

            namespace get_parameters_declaration
            {
                namespace missing_parameter_declaration
                {
                    auto parameters(const token_container_t& list_tokens)
                    {
                        token_container_t result{ token_open_paren() };

                        std::copy(std::cbegin(list_tokens), std::cend(list_tokens),
                                  std::back_inserter(result));

                        result.emplace_back(token_close_paren());

                        return result;
                    }

                    using MissingParameterDeclaration = testing::TestWithParam<token_container_t>;

                    TEST_P(MissingParameterDeclaration, ReportError)
                    {
                        report_error_test(GetParam(), [](auto& parser)
                        {
                            builtin_ctx_t ctx;
                            (void)parser.get_parameters_declaration(ctx);
                        });
                    }

                    const auto values = testing::Values(
                        parameters({ token_semicolon() }), // (,)
                        parameters({ token_identifier(), token_identifier(), token_semicolon() }), //(type name,)
                        parameters({ token_semicolon(), token_identifier() , token_identifier() }), //(,type name)
                        parameters({ token_identifier(), token_identifier(), token_semicolon(), token_semicolon(), token_identifier(), token_identifier() }) //(type name,,type name)
                    );

                    INSTANTIATE_TEST_CASE_P(ParserError_GetParametersDeclaration, MissingParameterDeclaration, values);
                }

                namespace missing_parenthesis
                {
                    using MissingParenthesis = testing::TestWithParam<token_container_t>;

                    TEST_P(MissingParenthesis, ReportError)
                    {
                        report_error_test(GetParam(), [](auto& parser)
                        {
                            builtin_ctx_t ctx;
                            (void)parser.get_parameters_declaration(ctx);
                        });
                    }

                    const auto values = testing::Values(
                        token_container_t{ token_open_paren() }, // (
                        token_container_t{ token_close_paren() }, // )
                        token_container_t{ token_open_paren(), token_identifier(), token_identifier() }, // (id id
                        token_container_t{ token_open_paren(), token_identifier(), token_identifier(), token_semicolon() }, // ( id id,
                        token_container_t{ token_identifier(), token_identifier(), token_close_paren() }, // id id)
                        token_container_t{ token_identifier(), token_identifier(), token_semicolon(), token_close_paren() } // id id,)
                    );

                    INSTANTIATE_TEST_CASE_P(ParserError_GetParametersDeclaration, MissingParenthesis, values);
                }
            }

            namespace get_infix_node
            {
                namespace unexpected_token
                {
                    using GetOnpExpression_UnexpectedToken = testing::TestWithParam<token_container_t>;

                    TEST_P(GetOnpExpression_UnexpectedToken, ReportError)
                    {
                        report_error_test(GetParam(), [](auto& parser)
                        {
                            (void)parser.get_infix();
                        });
                    }

                    const auto values = testing::Values(
                        token_container_t{token_kw_int() },
                        token_container_t{token_kw_double() }
                    );

                    INSTANTIATE_TEST_CASE_P(ParserError, GetOnpExpression_UnexpectedToken, values);
                }

                namespace missing_semicolon
                {
                    using GetOnpExpression_MissingSemicolon = testing::TestWithParam<token_container_t>;

                    TEST_P(GetOnpExpression_MissingSemicolon, ReportError)
                    {
                        report_error_test(GetParam(), [](auto& parser)
                        {
                            (void)parser.get_infix();
                        });
                    }

                    const auto values = testing::Values(
                        token_container_t{ token_integer(), token_plus(), token_integer() }
                    );

                    INSTANTIATE_TEST_CASE_P(ParserError, GetOnpExpression_MissingSemicolon, values);
                }
            }

            namespace get_block_node
            {
                namespace wrong_infix_node
                {
                    using GetBlockExpression_WrongOnpExpression = testing::TestWithParam<token_container_t>;

                    TEST_P(GetBlockExpression_WrongOnpExpression, ReportError)
                    {
                        report_error_test(GetParam(), [](auto& parser)
                        {
                            builtin_ctx_t ctx;
                            (void)parser.get_block(ctx);
                        });
                    }

                    const auto values = testing::Values(
                        token_container_t{token_kw_int() },
                        token_container_t{ token_integer(), token_plus(), token_integer() }
                    );

                    INSTANTIATE_TEST_CASE_P(ParserError, GetBlockExpression_WrongOnpExpression, values);
                }

                namespace missing_brace
                {
                    using GetBlockExpression_MissingBrace = testing::TestWithParam<token_container_t>;

                    TEST_P(GetBlockExpression_MissingBrace, ReportError)
                    {
                        report_error_test(GetParam(), [](auto& parser)
                        {
                            builtin_ctx_t ctx;
                            (void)parser.get_block(ctx);
                        });
                    }

                    const auto values = testing::Values(
                        token_container_t{ token_open_brace() },
                        token_container_t{ token_close_brace() },
                        token_container_t{ token_open_brace(), token_integer() },
                        token_container_t{ token_integer(), token_close_brace() }
                    );

                    INSTANTIATE_TEST_CASE_P(ParserError, GetBlockExpression_MissingBrace, values);
                }
            }

            namespace get_function
            {
                TEST(ParserError, GetFunction_MissingReturnType_ReportError)
                {
                    const auto tokens = token_container_t{ token_identifier(), token_open_paren(), token_close_paren(), token_open_brace(), token_close_brace() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_function(ctx);
                    });
                }

                TEST(ParserError, GetFunction_MissingName_ReportError)
                {
                    const auto tokens = token_container_t{token_kw_int(), token_open_paren(), token_close_paren(), token_open_brace(), token_close_brace() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_function(ctx);
                    });
                }

                TEST(ParserError, GetFunction_MissingParameters_ReportError)
                {
                    const auto tokens = token_container_t{token_kw_int(), token_identifier(), token_open_brace(), token_close_brace() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_function(ctx);
                    });
                }

                TEST(ParserError, GetFunction_MissingBody_ReportError)
                {
                    const auto tokens = token_container_t{token_kw_int(), token_identifier(), token_open_paren(), token_close_paren() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_function(ctx);
                    });
                }
            }

            namespace get_class_member_declaration
            {
                TEST(ParserError, GetClassMemberDeclaration_MissingType_ReportError)
                {
                    const auto tokens = token_container_t{ token_identifier(), token_semicolon() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_class_member_declaration(ctx);
                    });
                }

                TEST(ParserError, GetClassMemberDeclaration_MissingName_ReportError)
                {
                    const auto tokens = token_container_t{token_kw_int(), token_semicolon() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_class_member_declaration(ctx);
                    });
                }

                TEST(ParserError, GetClassMemberDeclaration_MissingSemicolon_ReportError)
                {
                    const auto tokens = token_container_t{
                            token_kw_int(), token_identifier(),
                        token_kw_int(), token_identifier(), token_semicolon() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_class_member_declaration(ctx);
                    });
                }

                TEST(ParserError, GetClassMemberDeclaration_WithIninMissingEqual_ReportError)
                {
                    const auto tokens = token_container_t{token_kw_int(), token_identifier(), token_integer(), token_semicolon() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_class_member_declaration(ctx);
                    });
                }

                TEST(ParserError, GetClassMemberDeclaration_WithEqualMissingExpr_ReportError)
                {
                    const auto tokens = token_container_t{token_kw_int(), token_identifier(), token_equal(), token_semicolon() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_class_member_declaration(ctx);
                    });
                }
            }

            namespace get_if_else_node
            {
                TEST(ParserError, GetIfElseNode_MissingConditionOpenParen_ReportError)
                {
                    // if 1) {}
                    const auto tokens = token_container_t{ token_kw_if(), token_integer("1"), token_close_paren(), token_open_brace(), token_close_brace() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_if_else_node(ctx);
                    });
                }

                TEST(ParserError, GetIfElseNode_MissingConditionCloseParen_ReportError)
                {
                    // if (1 {}
                    const auto tokens = token_container_t{ token_kw_if(), token_open_paren(), token_integer("1"), token_open_brace(), token_close_brace() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_if_else_node(ctx);
                    });
                }

                TEST(ParserError, GetIfElseNode_MissingBlockOpenBrace_ReportError)
                {
                    // if (1) }
                    const auto tokens = token_container_t{ token_kw_if(), token_open_paren(), token_integer("1"), token_close_paren(), token_close_brace() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_if_else_node(ctx);
                    });
                }

                TEST(ParserError, GetIfElseNode_MissingBlockCloseBrace_ReportError)
                {
                    // if (1) {
                    const auto tokens = token_container_t{ token_kw_if(), token_open_paren(), token_integer("1"), token_close_paren(), token_open_brace() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_if_else_node(ctx);
                    });
                }
            }

            namespace get_while_node
            {
                TEST(ParserError, GetWhileNode_MissingConditionOpenParen_ReportError)
                {
                    // while 1) {}
                    const auto tokens = token_container_t{ token_kw_while(), token_integer("1"), token_close_paren(), token_open_brace(), token_close_brace() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_while_node(ctx);
                    });
                }

                TEST(ParserError, GetWhileNode_MissingConditionCloseParen_ReportError)
                {
                    // while (1 {}
                    const auto tokens = token_container_t{ token_kw_while(), token_open_paren(), token_integer("1"), token_open_brace(), token_close_brace() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_while_node(ctx);
                    });
                }

                TEST(ParserError, GetWhileNode_MissingBlockOpenBrace_ReportError)
                {
                    // while (1) }
                    const auto tokens = token_container_t{ token_kw_while(), token_open_paren(), token_integer("1"), token_close_paren(), token_close_brace() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_while_node(ctx);
                    });
                }

                TEST(ParserError, GetWhileNode_MissingBlockCloseBrace_ReportError)
                {
                    // while (1) {
                    const auto tokens = token_container_t{ token_kw_while(), token_open_paren(), token_integer("1"), token_close_paren(), token_open_brace() };
                    report_error_test(tokens, [](auto& parser)
                    {
                        builtin_ctx_t ctx;
                        (void)parser.get_while_node(ctx);
                    });
                }
            }
        }
    }
}
