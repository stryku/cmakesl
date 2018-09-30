#include "exec/infix/infix_expression_builder.hpp"
#include "exec/infix/infix_expression.hpp"

// These needs to be included because gmock needs complete types
#include "exec/infix/class_member_access_expression.hpp"
#include "exec/infix/user_function_call_expression.hpp"
#include "exec/infix/binary_operator_expression.hpp"
#include "exec/infix/user_member_function_call_expression.hpp"
#include "exec/infix/id_expression.hpp"
#include "exec/infix/fundamental_value_expression.hpp"
#include "ast/function.hpp"
#include "exec/instance/instance.hpp"


#include "test/exec/mock/infix_expression_visitor_mock.hpp"
#include "test/exec/mock/function_caller_mock.hpp"
#include "test/common/tokens.hpp"

namespace cmsl
{
    namespace test
    {
        namespace exec
        {
            namespace infix
            {
                namespace infix_expression_builder
                {
                    using testing::_;
                    using testing::TypedEq;
                    using testing::Matcher;

                    using builder_t = cmsl::exec::infix::infix_expression_builder;
                    using tokens_t = cmsl::lexer::token::token_container_t;

                    using namespace cmsl::test::common;

                    class InfixExpressionBuilder : public testing::Test
                    {
                    protected:
                        template <typename Expression>
                        void test_single_expression(tokens_t tokens)
                        {
                            function_caller_mock fun_caller;
                            auto builder = builder_t{ fun_caller, tokens };
                            auto expr = builder.build();

                            infix_expression_visitor_mock visitor_mock;
                            EXPECT_CALL(visitor_mock, visit(Matcher<const Expression&>(_)));

                            expr->visit(visitor_mock);
                        }
                    };

                    TEST_F(InfixExpressionBuilder, TrueFalseKeyword_GetBoolFundamentalValue)
                    {
                        using bool_expr_t = cmsl::exec::infix::bool_value_expression;
                        test_single_expression<bool_expr_t>(tokens_t{ token_kw_true() });
                        test_single_expression<bool_expr_t>(tokens_t{ token_kw_false() });
                    }

                    TEST_F(InfixExpressionBuilder, String_GetFundamentalStringValue)
                    {
                        using string_expr_t = cmsl::exec::infix::string_value_expression;
                        test_single_expression<string_expr_t>(tokens_t{ token_string("\"\"") });
                    }

                    TEST_F(InfixExpressionBuilder, Int_GetFundamentalIntValue)
                    {
                        using int_expr_t = cmsl::exec::infix::int_value_expression;
                        test_single_expression<int_expr_t>(tokens_t{ token_integer("1") });
                    }

                    TEST_F(InfixExpressionBuilder, Duble_GetFundamentalIntValue)
                    {
                        using double_expr_t = cmsl::exec::infix::double_value_expression;
                        test_single_expression<double_expr_t>(tokens_t{ token_double("1.0") });
                    }

                    TEST_F(InfixExpressionBuilder, Identifer_GetId)
                    {
                        using id_expr_t = cmsl::exec::infix::id_expression;
                        test_single_expression<id_expr_t>(tokens_t{ token_identifier("foo") });
                    }

                    TEST_F(InfixExpressionBuilder, IdentiferDotIdentifer_GetClassMemberAccess)
                    {
                        using member_access_expr_t = cmsl::exec::infix::class_member_access_expression;
                        const auto tokens = tokens_t{ token_identifier("foo"), token_dot(), token_identifier("bar") };
                        test_single_expression<member_access_expr_t>(tokens);
                    }

                    TEST_F(InfixExpressionBuilder, ComplexLhsDotIdentifer_GetClassMemberAccess)
                    {
                        using member_access_expr_t = cmsl::exec::infix::class_member_access_expression;
                        const auto tokens = tokens_t{ token_open_paren(), token_identifier("foo"), token_close_paren(),
                                                      token_dot(), token_identifier("bar") };
                        test_single_expression<member_access_expr_t>(tokens);
                    }

                    TEST_F(InfixExpressionBuilder, IdentiferDotIdentiferParens_GetMemberFunctionCall)
                    {
                        using member_fun_call_expr_t = cmsl::exec::infix::user_member_function_call_expression;
                        const auto tokens = tokens_t{ token_identifier("foo"), token_dot(), token_identifier("bar"), token_open_paren(), token_close_paren() };
                        test_single_expression<member_fun_call_expr_t>(tokens);
                    }

                    TEST_F(InfixExpressionBuilder, IdentiferDotIdentiferParensWithParam_GetMemberFunctionCall)
                    {
                        using member_fun_call_expr_t = cmsl::exec::infix::user_member_function_call_expression;
                        const auto tokens = tokens_t{ token_identifier("foo"), token_dot(), token_identifier("bar"),
                                                      token_open_paren(),
                                                      token_identifier("baz"),
                                                      token_close_paren() };
                        test_single_expression<member_fun_call_expr_t>(tokens);
                    }

                    TEST_F(InfixExpressionBuilder, IdentiferDotIdentiferParensWithParams_GetMemberFunctionCall)
                    {
                        using member_fun_call_expr_t = cmsl::exec::infix::user_member_function_call_expression;
                        const auto tokens = tokens_t{ token_identifier("foo"), token_dot(), token_identifier("bar"),
                                                      token_open_paren(),
                                                      token_identifier("baz"),
                                                      token_comma(),
                                                      token_identifier("qux"),
                                                      token_close_paren() };
                        test_single_expression<member_fun_call_expr_t>(tokens);
                    }
                }
            }
        }
    }
}
