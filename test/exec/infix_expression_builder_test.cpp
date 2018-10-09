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


#include "test/exec/fake/fake_function_caller.hpp"
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

                    using tree_representation_t = decltype(std::declval<InfixTreeRepresentationVisitor>().get_tree());


                    auto generate_tree_representations(std::unique_ptr<infix_expression> expected, tokens_t test_tokens)
                    {
                        auto expected_visitor = InfixTreeRepresentationVisitor{};
                        expected->visit(expected_visitor);
                        const auto expected_tree_representation = expected_visitor.get_tree();

                        exec::test::function_caller_mock fun_caller;
                        auto builder = infix_expression_builder{fun_caller,
                                                                test_tokens};
                        auto expr = builder.build();
                        auto built_visitor = InfixTreeRepresentationVisitor{};
                        expr->visit(built_visitor);
                        const auto built_tree_representation = built_visitor.get_tree();

                        return std::make_pair(expected_tree_representation, built_tree_representation);
                    };


                    struct complex_call_expression_values
                    {
                        token_t function_name;
                        call_expression::params_t params;
                        tokens_t call_tokens;
                    };

                    // creates values to test against: bar(1, 2 + 3, 4 * (5 - 6), baz())
                    auto get_complex_call_expression_values()
                    {
                        auto lhs = std::make_unique<id_expression>(token_identifier("foo"));
                        auto function_name = token_identifier("bar");
                        const auto dot = token_dot();
                        auto caller = exec::test::fake_function_caller{};

                        auto param1 = std::make_unique<int_value_expression>(token_integer("1"));

                        auto param2_lhs = std::make_unique<int_value_expression>(token_integer("2"));
                        auto param2_rhs = std::make_unique<int_value_expression>(token_integer("3"));
                        auto param2 = std::make_unique<binary_operator_expression>(std::move(param2_lhs), token_plus(), std::move(param2_rhs));

                        auto param3_lhs = std::make_unique<int_value_expression>(token_integer("4"));
                        auto param3_rhs_lhs = std::make_unique<int_value_expression>(token_integer("5"));
                        auto param3_rhs_rhs = std::make_unique<int_value_expression>(token_integer("6"));
                        auto param3_rhs = std::make_unique<binary_operator_expression>(std::move(param3_rhs_lhs), token_minus(), std::move(param3_rhs_rhs));
                        auto param3 = std::make_unique<binary_operator_expression>(std::move(param3_lhs), token_star(), std::move(param3_rhs));

                        auto param4 = std::make_unique<function_call_expression>(caller, token_identifier("baz"), call_expression::params_t{});

                        // todo brace initialize
                        call_expression::params_t params;
                        params.emplace_back(std::move(param1));
                        params.emplace_back(std::move(param2));
                        params.emplace_back(std::move(param3));
                        params.emplace_back(std::move(param4));

                        const auto test_tokens = tokens_t{token_identifier("bar"),
                                                          token_open_paren(),

                                                          token_integer("1"),
                                                          token_comma(),

                                                          token_integer("2"),
                                                          token_plus(),
                                                          token_integer("3"),
                                                          token_comma(),

                                                          token_integer("4"),
                                                          token_star(),
                                                          token_open_paren(),
                                                          token_integer("5"),
                                                          token_plus(),
                                                          token_integer("6"),
                                                          token_close_paren(),
                                                          token_comma(),

                                                          token_identifier("baz"),
                                                          token_open_paren(),
                                                          token_close_paren(),

                                                          token_close_paren()};

                        return complex_call_expression_values{ function_name, std::move(params), test_tokens };
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
                    // tests: 1 + 2

                    auto lhs = make_int("1");
                    auto rhs = make_int("2");
                    const auto plus = token_plus();
                    auto expected_expr = std::make_unique<binary_operator_expression>(std::move(lhs), plus, std::move(rhs));

                    const auto test_tokens = tokens_t{token_integer("1"),
                                                      plus,
                                                      token_integer("2")};

                    const auto representations = generate_tree_representations(std::move(expected_expr), test_tokens);
                    EXPECT_THAT(representations.first, Eq(representations.second));
                }

                TEST_F(InfixExpressionBuilder, MultipleBinaryOperatorsWithSamePrecedence)
                {
                    // test: 1 + 2 - 3

                    auto lhs_lhs = make_int("1");
                    auto lhs_rhs = make_int("2");
                    auto rhs = make_int("3");
                    const auto plus = token_plus();
                    const auto minus = token_minus();
                    auto lhs = std::make_unique<binary_operator_expression>(std::move(lhs_lhs), plus, std::move(lhs_rhs));
                    auto expected_expr = std::make_unique<binary_operator_expression>(std::move(lhs), minus, std::move(rhs));

                    const auto test_tokens = tokens_t{token_integer("1"),
                                                      token_plus(),
                                                      token_integer("2"),
                                                      token_minus(),
                                                      token_integer("3")};

                    const auto representations = generate_tree_representations(std::move(expected_expr), test_tokens);
                    EXPECT_THAT(representations.first, Eq(representations.second));
                }

                TEST_F(InfixExpressionBuilder, MultipleBinaryOperatorsParens)
                {
                    // test: (1 + 2) * 3

                    auto lhs_lhs = make_int("1");
                    auto lhs_rhs = make_int("2");
                    auto rhs = make_int("3");
                    const auto star = token_star();
                    const auto plus = token_plus();
                    auto lhs = std::make_unique<binary_operator_expression>(std::move(lhs_lhs), plus,
                                                                            std::move(lhs_rhs));
                    auto expected_expr = std::make_unique<binary_operator_expression>(std::move(lhs), star,
                                                                                      std::move(rhs));

                    const auto test_tokens = tokens_t{token_open_paren(),
                                                      token_integer("1"),
                                                      plus,
                                                      token_integer("2"),
                                                      token_close_paren(),
                                                      star,
                                                      token_integer("3")};

                    const auto representations = generate_tree_representations(std::move(expected_expr),
                                                                               test_tokens);
                    EXPECT_THAT(representations.first, Eq(representations.second));
                }

                TEST_F(InfixExpressionBuilder, MultipleBinaryOperatorsWithDifferentPrecedences)
                {
                    {
                        // test: 1 + 2 * 3

                        auto lhs = make_int("1");
                        auto rhs_lhs = make_int("2");
                        auto rhs_rhs = make_int("3");
                        const auto plus = token_plus();
                        const auto star = token_star();
                        auto rhs = std::make_unique<binary_operator_expression>(std::move(rhs_lhs), star,
                                                                                std::move(rhs_rhs));
                        auto expected_expr = std::make_unique<binary_operator_expression>(std::move(lhs), plus,
                                                                                          std::move(rhs));

                        const auto test_tokens = tokens_t{token_integer("1"),
                                                          plus,
                                                          token_integer("2"),
                                                          star,
                                                          token_integer("3")};

                        const auto representations = generate_tree_representations(std::move(expected_expr),
                                                                                   test_tokens);
                        EXPECT_THAT(representations.first, Eq(representations.second));
                    }
                    {
                        // test: 1 * 2 + 3

                        auto lhs_lhs = make_int("1");
                        auto lhs_rhs = make_int("2");
                        auto rhs = make_int("3");
                        const auto star = token_star();
                        const auto plus = token_plus();
                        auto lhs = std::make_unique<binary_operator_expression>(std::move(lhs_lhs), star,
                                                                                std::move(lhs_rhs));
                        auto expected_expr = std::make_unique<binary_operator_expression>(std::move(lhs), plus,
                                                                                          std::move(rhs));

                        const auto test_tokens = tokens_t{token_integer("1"),
                                                          star,
                                                          token_integer("2"),
                                                          plus,
                                                          token_integer("3")};

                        const auto representations = generate_tree_representations(std::move(expected_expr),
                                                                                   test_tokens);
                        EXPECT_THAT(representations.first, Eq(representations.second));
                    }
                }

                TEST_F(InfixExpressionBuilder, ClassMemberAccess)
                {
                    // tests: foo.bar

                    auto lhs = std::make_unique<id_expression>(token_identifier("foo"));
                    auto member_name = token_identifier("bar");
                    const auto dot = token_dot();
                    auto expected_expr = std::make_unique<class_member_access_expression>(std::move(lhs), member_name);

                    const auto test_tokens = tokens_t{token_identifier("foo"),
                                                      dot,
                                                      token_identifier("bar")};

                    const auto representations = generate_tree_representations(std::move(expected_expr), test_tokens);
                    EXPECT_THAT(representations.first, Eq(representations.second));
                }

                TEST_F(InfixExpressionBuilder, ClassMemberAccessComplexLhs)
                {
                    // tests: (foo + bar).bar

                    auto lhs_lhs = std::make_unique<id_expression>(token_identifier("foo"));
                    auto lhs_rhs = std::make_unique<id_expression>(token_identifier("bar"));
                    auto member_name = token_identifier("baz");
                    const auto dot = token_dot();
                    const auto plus = token_plus();
                    auto lhs = std::make_unique<binary_operator_expression>(std::move(lhs_lhs), plus,
                                                                            std::move(lhs_rhs));
                    auto expected_expr = std::make_unique<class_member_access_expression>(std::move(lhs), member_name);

                    const auto test_tokens = tokens_t{token_open_paren(),
                                                      token_identifier("foo"),
                                                      plus,
                                                      token_identifier("bar"),
                                                      token_close_paren(),
                                                      dot,
                                                      token_identifier("baz")};

                    const auto representations = generate_tree_representations(std::move(expected_expr), test_tokens);
                    EXPECT_THAT(representations.first, Eq(representations.second));
                }

                TEST_F(InfixExpressionBuilder, MemberFunctionCall)
                {
                    // tests: foo.bar()

                    auto lhs = std::make_unique<id_expression>(token_identifier("foo"));
                    auto function_name = token_identifier("bar");
                    const auto dot = token_dot();
                    auto caller = exec::test::fake_function_caller{};
                    auto expected_expr = std::make_unique<member_function_call_expression>(caller, std::move(lhs), function_name, member_function_call_expression::params_t{});

                    const auto test_tokens = tokens_t{token_identifier("foo"),
                                                      dot,
                                                      token_identifier("bar"),
                                                      token_open_paren(),
                                                      token_close_paren()};

                    const auto representations = generate_tree_representations(std::move(expected_expr), test_tokens);
                    EXPECT_THAT(representations.first, Eq(representations.second));
                }

                TEST_F(InfixExpressionBuilder, MemberFunctionCallCorrectPrecedence)
                {
                    // tests: 1 + 2.to_string()

                    auto one = std::make_unique<int_value_expression>(token_integer("1"));
                    auto lhs = std::make_unique<int_value_expression>(token_integer("2"));
                    auto function_name = token_identifier("to_string");
                    auto caller = exec::test::fake_function_caller{};
                    auto call_expr = std::make_unique<member_function_call_expression>(caller, std::move(lhs), function_name, member_function_call_expression::params_t{});
                    auto expected_expr = std::make_unique<binary_operator_expression>(std::move(one), token_plus(), std::move(call_expr));

                    const auto test_tokens = tokens_t{token_integer("1"),
                                                      token_plus(),
                                                      token_integer("2"),
                                                      token_dot(),
                                                      token_identifier("to_string"),
                                                      token_open_paren(),
                                                      token_close_paren()};

                    const auto representations = generate_tree_representations(std::move(expected_expr), test_tokens);
                    EXPECT_THAT(representations.first, Eq(representations.second));
                }

                TEST_F(InfixExpressionBuilder, MemberFunctionCallParams)
                {
                    // tests: foo.bar(1, 2 + 3, 4 * (5 - 6), baz())
                    auto caller = exec::test::fake_function_caller{};
                    auto call_values = get_complex_call_expression_values();

                    auto lhs = std::make_unique<id_expression>(token_identifier("foo"));
                    auto expected_expr = std::make_unique<member_function_call_expression>(caller, std::move(lhs), call_values.function_name, std::move(call_values.params));

                    auto test_tokens = tokens_t{ token_identifier("foo"), token_dot() };
                    test_tokens.insert(test_tokens.end(), call_values.call_tokens.cbegin(), call_values.call_tokens.cend());


                    const auto representations = generate_tree_representations(std::move(expected_expr), test_tokens);
                    EXPECT_THAT(representations.first, Eq(representations.second));
                }

                TEST_F(InfixExpressionBuilder, FunctionCall)
                {
                    // tests: foo()
                    auto caller = exec::test::fake_function_caller{};

                    auto expected_expr = std::make_unique<function_call_expression>(caller, token_identifier("foo"), call_expression::params_t{});

                    const auto test_tokens = tokens_t{ token_identifier("foo"),
                                                       token_open_paren(),
                                                       token_close_paren()
                    };

                    const auto representations = generate_tree_representations(std::move(expected_expr), test_tokens);
                    EXPECT_THAT(representations.first, Eq(representations.second));
                }

                TEST_F(InfixExpressionBuilder, FunctionCallParams)
                {
                    // tests: bar(1, 2 + 3, 4 * (5 - 6), baz())
                    auto caller = exec::test::fake_function_caller{};
                    auto call_values = get_complex_call_expression_values();

                    auto expected_expr = std::make_unique<function_call_expression>(caller, call_values.function_name, std::move(call_values.params));

                    const auto representations = generate_tree_representations(std::move(expected_expr), call_values.call_tokens);
                    EXPECT_THAT(representations.first, Eq(representations.second));
                }
            }
        }
    }
}
