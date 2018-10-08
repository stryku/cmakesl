#include "exec/infix/infix_expression_builder.hpp"
#include "exec/infix/infix_expression.hpp"

// These needs to be included because gmock needs complete types
#include "exec/infix/class_member_access_expression.hpp"
#include "exec/infix/function_call_expression.hpp"
#include "exec/infix/binary_operator_expression.hpp"
#include "exec/infix/member_function_call_expression.hpp"
#include "exec/infix/id_expression.hpp"
#include "exec/infix/fundamental_value_expression.hpp"
#include "ast/function.hpp"
#include "exec/instance/instance.hpp"


#include "test/exec/mock/infix_expression_visitor_mock.hpp"
#include "test/exec/mock/function_caller_mock.hpp"
#include "test/common/tokens.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            namespace test
            {
                using testing::_;
                using testing::TypedEq;
                using testing::Matcher;
                using testing::Eq;

                using tokens_t = lexer::token::token_container_t;

                using namespace cmsl::test::common;

                class InfixTreeRepresentationVisitor : public infix_expression_visitor
                {
                public:
                    void visit(const bool_value_expression& expr) override
                    {
                        m_flat_tree += "bool_value{" + expr.get_token().str().to_string() + "}";
                    }

                    void visit(const int_value_expression& expr) override
                    {
                        m_flat_tree += "int_value{" + expr.get_token().str().to_string() + "}";
                    }

                    void visit(const double_value_expression& expr) override
                    {
                        m_flat_tree += "double_value{" + expr.get_token().str().to_string() + "}";
                    }

                    void visit(const string_value_expression& expr) override
                    {
                        m_flat_tree += "string_value{" + expr.get_token().str().to_string() + "}";
                    }

                    void visit(const id_expression& expr) override
                    {
                        m_flat_tree += "id{" + expr.get_identifier().str().to_string() + '}';
                    }

                    void visit(const member_function_call_expression& expr) override
                    {
                        m_flat_tree += "member_function_call{";

                        m_flat_tree += "{lhs:";
                        expr.get_lhs().visit(*this);
                        m_flat_tree += '}';

                        m_flat_tree += "name{" + expr.get_name().str().to_string() + '}';
                        for(const auto param : expr.get_param_expressions())
                        {
                            m_flat_tree += "param{";
                            param->visit(*this);
                            m_flat_tree += '}';
                        }

                        m_flat_tree += '}';
                    }

                    void visit(const binary_operator_expression& expr) override
                    {
                        m_flat_tree += "binary_operator{";

                        m_flat_tree += "{lhs:";
                        expr.get_lhs().visit(*this);
                        m_flat_tree += '}';

                        m_flat_tree += "{op:" + expr.get_operator().str().to_string() + '}';

                        m_flat_tree += "{lhs:";
                        expr.get_lhs().visit(*this);
                        m_flat_tree += '}';

                        m_flat_tree += '}';
                    }

                    void visit(const function_call_expression& expr) override
                    {
                        m_flat_tree += "function_call{";
                        m_flat_tree += "name{" + expr.get_name().str().to_string() + '}';
                        for(const auto param : expr.get_param_expressions())
                        {
                            m_flat_tree += "param{";
                            param->visit(*this);
                            m_flat_tree += '}';
                        }
                        m_flat_tree += '}';
                    }

                    void visit(const class_member_access_expression& expr) override
                    {
                        m_flat_tree += "class_member_access{";
                        m_flat_tree += "lhs{";
                        expr.get_lhs().visit(*this);
                        m_flat_tree += "}member_name{" + expr.get_member_name().str().to_string() + "}}";
                    }

                    std::string get_tree() const
                    {
                        return m_flat_tree;
                    }

                private:
                    std::string m_flat_tree;
                };

                class InfixExpressionBuilder : public testing::Test
                {
                protected:
                    template<typename Expression>
                    void test_single_expression(tokens_t tokens)
                    {
                        exec::test::function_caller_mock fun_caller;
                        auto builder = infix_expression_builder{fun_caller,
                                                                tokens};
                        auto expr = builder.build();

                        infix::test::infix_expression_visitor_mock visitor_mock;
                        EXPECT_CALL(visitor_mock, visit(Matcher<const Expression &>(_)));

                        expr->visit(visitor_mock);
                    }

                    std::unique_ptr<infix_expression> make_int(cmsl::string_view value)
                    {
                        return std::make_unique<int_value_expression>(token_integer(value));
                    }
                };

                TEST_F(InfixExpressionBuilder, TrueFalseKeyword_GetBoolFundamentalValue)
                {
                    using bool_expr_t = cmsl::exec::infix::bool_value_expression;
                    test_single_expression<bool_expr_t>(tokens_t{token_kw_true()});
                    test_single_expression<bool_expr_t>(tokens_t{token_kw_false()});
                }

                TEST_F(InfixExpressionBuilder, String_GetFundamentalStringValue)
                {
                    using string_expr_t = cmsl::exec::infix::string_value_expression;
                    test_single_expression<string_expr_t>(tokens_t{token_string("\"\"")});
                }

                TEST_F(InfixExpressionBuilder, Int_GetFundamentalIntValue)
                {
                    using int_expr_t = cmsl::exec::infix::int_value_expression;
                    test_single_expression<int_expr_t>(tokens_t{token_integer("1")});
                }

                TEST_F(InfixExpressionBuilder, Duble_GetFundamentalIntValue)
                {
                    using double_expr_t = cmsl::exec::infix::double_value_expression;
                    test_single_expression<double_expr_t>(tokens_t{token_double("1.0")});
                }

                TEST_F(InfixExpressionBuilder, Identifer_GetId)
                {
                    using id_expr_t = cmsl::exec::infix::id_expression;
                    test_single_expression<id_expr_t>(tokens_t{token_identifier("foo")});
                }

                TEST_F(InfixExpressionBuilder, IdentiferDotIdentifer_GetClassMemberAccess)
                {
                    using member_access_expr_t = cmsl::exec::infix::class_member_access_expression;
                    const auto tokens = tokens_t{token_identifier("foo"),
                                                 token_dot(),
                                                 token_identifier("bar")};
                    test_single_expression<member_access_expr_t>(tokens);
                }

                TEST_F(InfixExpressionBuilder, ComplexLhsDotIdentifer_GetClassMemberAccess)
                {
                    using member_access_expr_t = cmsl::exec::infix::class_member_access_expression;
                    const auto tokens = tokens_t{token_open_paren(),
                                                 token_identifier("foo"),
                                                 token_close_paren(),
                                                 token_dot(),
                                                 token_identifier("bar")};
                    test_single_expression<member_access_expr_t>(tokens);
                }

                TEST_F(InfixExpressionBuilder, IdentiferDotIdentiferParens_GetMemberFunctionCall)
                {
                    using member_fun_call_expr_t = cmsl::exec::infix::member_function_call_expression;
                    const auto tokens = tokens_t{token_identifier("foo"),
                                                 token_dot(),
                                                 token_identifier("bar"),
                                                 token_open_paren(),
                                                 token_close_paren()};
                    test_single_expression<member_fun_call_expr_t>(tokens);
                }

                TEST_F(InfixExpressionBuilder, IdentiferDotIdentiferParensWithParam_GetMemberFunctionCall)
                {
                    using member_fun_call_expr_t = cmsl::exec::infix::member_function_call_expression;
                    const auto tokens = tokens_t{token_identifier("foo"),
                                                 token_dot(),
                                                 token_identifier("bar"),
                                                 token_open_paren(),
                                                 token_identifier("baz"),
                                                 token_close_paren()};
                    test_single_expression<member_fun_call_expr_t>(tokens);
                }

                TEST_F(InfixExpressionBuilder, IdentiferDotIdentiferParensWithParams_GetMemberFunctionCall)
                {
                    using member_fun_call_expr_t = cmsl::exec::infix::member_function_call_expression;
                    const auto tokens = tokens_t{token_identifier("foo"),
                                                 token_dot(),
                                                 token_identifier("bar"),
                                                 token_open_paren(),
                                                 token_identifier("baz"),
                                                 token_comma(),
                                                 token_identifier("qux"),
                                                 token_close_paren()};
                    test_single_expression<member_fun_call_expr_t>(tokens);
                }

                TEST_F(InfixExpressionBuilder, BinaryOperator)
                {
                    auto lhs = make_int("1");
                    auto rhs = make_int("2");
                    const auto op = token_plus();
                    auto expected_tree = std::make_unique<binary_operator_expression>(std::move(lhs), op, std::move(rhs));

                    auto expected_visitor = InfixTreeRepresentationVisitor{};
                    expected_tree->visit(expected_visitor);
                    const auto expected_tree_representation = expected_visitor.get_tree();

                    const auto tokens = tokens_t{token_integer("1"),
                                                 token_plus(),
                                                 token_integer("2")};
                    exec::test::function_caller_mock fun_caller;
                    auto builder = infix_expression_builder{fun_caller,
                                                            tokens};
                    auto expr = builder.build();
                    auto built_visitor = InfixTreeRepresentationVisitor{};
                    expr->visit(built_visitor);
                    const auto built_tree_representation = built_visitor.get_tree();

                    EXPECT_THAT(built_tree_representation, Eq(expected_tree_representation));
                }
            }
        }
    }
}
