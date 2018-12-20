#include "ast/ast_node_visitor.hpp"
#include "ast/infix_nodes.hpp"
#include "ast/return_node.hpp"
#include "ast/block_node.hpp"
#include "ast/class_node.hpp"
#include "ast/conditional_node.hpp"
#include "ast/if_else_node.hpp"
#include "ast/translation_unit_node.hpp"
#include "ast/variable_declaration_node.hpp"
#include "ast/while_node.hpp"
#include "ast/parser2.hpp"

#include "errors/errors_observer.hpp"

#include "test/common/tokens.hpp"

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

            using namespace cmsl::test::common;
            errors::errors_observer dummy_err_observer;

            using ::testing::NotNull;
            using ::testing::Ref;
            using ::testing::Eq;

            class ast_tree_representation_visitor : public ast_node_visitor
            {
            public:
                virtual void visit(const block_node& node) override
                {
                    m_result += "block{";
                    for(auto n : node.get_expressions())
                    {
                        n->visit(*this);
                    }
                    m_result += "}";
                }

                virtual void visit(const class_node2& node) override
                {
                    m_result += "class{name:" + node.get_name().str().to_string() + ";members:";

                    for(auto n : node.get_nodes())
                    {
                        n->visit(*this);
                    }

                    m_result += "}";
                }

                virtual void visit(const conditional_node& node) override
                {
                    m_result += "conditional{condition:";
                    node.get_condition().visit(*this);
                    m_result += ";block:";
                    node.get_block().visit(*this);
                    m_result += "}";
                }

                virtual void visit(const if_else_node& node) override
                {
                    m_result += "if_else{ifs:";
                    for (const auto &if_ : node.get_ifs())
                    {
                        if_->visit(*this);
                    }
                    m_result += ";else:";
                    auto else_node = node.get_else();
                    if (else_node)
                    {
                        else_node->visit(*this);
                    }
                    m_result += "}";
                }

                virtual void visit(const binary_operator_node& node) override
                {
                    m_result += "binary_operator{lhs:";
                    node.get_lhs().visit(*this);
                    m_result += ";operator:" + node.get_operator().str().to_string() + ";rhs:";
                    node.get_rhs().visit(*this);
                    m_result += "}";
                }

                virtual void visit(const class_member_access_node& node) override
                {
                    m_result += "class_member_access{lhs:";
                    node.get_lhs().visit(*this);
                    m_result += ";member_name:" + node.get_member_name().str().to_string() + "}";
                }

                virtual void visit(const function_call_node& node) override
                {
                    m_result += "function_call{name:" + node.get_name().str().to_string() + ";params:";
                    for(const auto& param : node.get_param_nodes())
                    {
                        param->visit(*this);
                    }
                    m_result += "}";
                }

                virtual void visit(const member_function_call_node& node) override
                {
                    m_result += "function_call{name:" + node.get_name().str().to_string() + ";params:";
                    m_result += "function_call{lhs:";
                    node.get_lhs().visit(*this);
                    m_result += ";name:" + node.get_name().str().to_string() + ";params:";
                    for(const auto& param : node.get_param_nodes())
                    {
                        param->visit(*this);
                    }
                    m_result += "}";
                }

                virtual void visit(const bool_value_node& node) override
                {
                    m_result += "bool_value{" + node.get_token().str().to_string() + "}";
                }

                virtual void visit(const int_value_node& node) override
                {
                    m_result += "int_value{" + node.get_token().str().to_string() + "}";
                }

                virtual void visit(const double_value_node& node) override
                {
                    m_result += "double_value{" + node.get_token().str().to_string() + "}";
                }

                virtual void visit(const string_value_node& node) override
                {
                    m_result += "string_value{" + node.get_token().str().to_string() + "}";
                }

                virtual void visit(const id_node& node) override
                {
                    m_result += "id{" + node.get_identifier().str().to_string() + "}";
                }

                virtual void visit(const return_node& node) override
                {
                    m_result += "return{";
                    node.get_expression().visit(*this);
                    m_result += "}";
                }

                virtual void visit(const translation_unit_node& node) override
                {
                    m_result += "translation_unit{";
                    for(const auto n : node.get_nodes())
                    {
                        n->visit(*this);
                    }
                    m_result += "}";
                }

                virtual void visit(const user_function_node2& node) override
                {
                    m_result += "user_function{return_type:";
                    const auto ret_type_reference = node.get_return_type_reference();
                    m_result += ret_type_reference.to_string().to_string() + ";name:";
                    m_result += node.get_name().str().to_string() + ";params:";
                    for(const auto& param_declaration : node.get_param_declarations())
                    {
                        m_result += "param_declaration{type:" + param_declaration.ty.to_string().to_string();
                        m_result += ";name:" + param_declaration.name.str().to_string() + "}";
                    }
                    m_result += ";body:";
                    node.get_body().visit(*this);
                    m_result += "}";
                }

                virtual void visit(const variable_declaration_node& node) override
                {
                    m_result += "variable_declaration{type:";
                    const auto ret_type_reference = node.get_type_reference();
                    m_result += ret_type_reference.to_string().to_string() + ";name:";
                    m_result += node.get_name().str().to_string() + ";initialization:";
                    if(const auto init = node.get_initialization())
                    {
                        init->visit(*this);
                    }
                    m_result += "}";
                }

                virtual void visit(const while_node& node) override
                {
                    m_result += "while{";
                    node.get_node().visit(*this);
                    m_result += "}";
                }

                std::string result() const
                {
                    return m_result;
                }

            private:
                std::string m_result;
            };

            MATCHER_P(AstEq, expected_ast, "")
            {
                ast_tree_representation_visitor result_visitor;
                arg->visit(result_visitor);

                ast_tree_representation_visitor expected_visitor;
                expected_ast->visit(expected_visitor);

                // Uncomment to see nice values at a test fail.
                *result_listener << "result: " << result_visitor.result() << "\nexpected: " << expected_visitor.result();

                return result_visitor.result() == expected_visitor.result();
            }

            TEST(Parser2Test, Factor_TrueKeywordToken_GetBoolFundamentalValue)
            {
                const auto token = token_kw_true();

                auto expected_ast = std::make_unique<bool_value_node>(token);

                const auto tokens = tokens_container_t{ token };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.factor();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Factor_FalseKeywordToken_GetBoolFundamentalValue)
            {
                const auto token = token_kw_false();

                auto expected_ast = std::make_unique<bool_value_node>(token);

                const auto tokens = tokens_container_t{ token };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.factor();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Factor_IntegerToken_GetIntFundamentalValue)
            {
                const auto token = token_integer("1");

                auto expected_ast = std::make_unique<int_value_node>(token);

                const auto tokens = tokens_container_t{ token };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.factor();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Factor_DoubleToken_GetDoubleFundamentalValue)
            {
                const auto token = token_double("1.6");

                auto expected_ast = std::make_unique<double_value_node>(token);

                const auto tokens = tokens_container_t{ token };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.factor();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Factor_StringToken_GetStringFundamentalValue)
            {
                const auto token = token_string("some string");

                auto expected_ast = std::make_unique<string_value_node>(token);

                const auto tokens = tokens_container_t{ token };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.factor();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Factor_IdentifierToken_GetId)
            {
                const auto token = token_identifier("some_name");

                auto expected_ast = std::make_unique<id_node>(token);

                const auto tokens = tokens_container_t{ token };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.factor();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Factor_ParenthesizedToken_GetInnerNode)
            {
                const auto lparen = token_open_paren();
                const auto id = token_identifier("some_name");
                const auto rparen = token_close_paren();

                auto expected_ast = std::make_unique<id_node>(id);

                const auto tokens = tokens_container_t{ lparen, id, rparen };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.factor();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Expr_TokenBinaryOpToken_GetBinaryOperator)
            {
                // 1 + 2
                const auto lhs_token = token_integer("1");
                const auto op_token = token_plus();
                const auto rhs_token = token_integer("2");

                auto lhs_node = std::make_unique<int_value_node>(lhs_token);
                auto rhs_node = std::make_unique<int_value_node>(rhs_token);
                auto expected_ast = std::make_unique<binary_operator_node>(std::move(lhs_node),
                                                                           op_token,
                                                                           std::move(rhs_node));

                const auto tokens = tokens_container_t{ lhs_token, op_token, rhs_token };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.expr();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Expr_TokenBinaryOpTokenBinaryOpToken_GetTree)
            {
                // "foo" - bar - 4.2
                const auto lhs_lhs_token = token_string("foo");
                const auto lhs_op_token = token_minus();
                const auto lhs_rhs_token = token_identifier("bar");
                const auto op_token = token_minus();
                const auto rhs_token = token_double("4.2");

                auto lhs_lhs_node = std::make_unique<string_value_node>(lhs_lhs_token);
                auto lhs_rhs_node = std::make_unique<id_node>(lhs_rhs_token);
                auto lhs_node = std::make_unique<binary_operator_node>(std::move(lhs_lhs_node),
                                                                       lhs_op_token,
                                                                       std::move(lhs_rhs_node));

                auto rhs_node = std::make_unique<double_value_node>(rhs_token);

                auto expected_ast = std::make_unique<binary_operator_node>(std::move(lhs_node),
                                                                           op_token,
                                                                           std::move(rhs_node));

                const auto tokens = tokens_container_t{ lhs_lhs_token, lhs_op_token, lhs_rhs_token, op_token, rhs_token };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.expr();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Expr_TokenLowPrecedenceOpTokenHighPrecedenceOpToken_GetTree)
            {
                // "foo" - bar * 4.2
                const auto lhs_token = token_string("foo");
                const auto op_token = token_minus();
                const auto rhs_lhs_token = token_identifier("bar");
                const auto rhs_op_token = token_star();
                const auto rhs_rhs_token = token_double("4.2");

                auto lhs_node = std::make_unique<string_value_node>(lhs_token);

                auto rhs_lhs_node = std::make_unique<id_node>(rhs_lhs_token);
                auto rhs_rhs_node = std::make_unique<double_value_node>(rhs_rhs_token);
                auto rhs_node = std::make_unique<binary_operator_node>(std::move(rhs_lhs_node),
                                                                       rhs_op_token,
                                                                       std::move(rhs_rhs_node));

                auto expected_ast = std::make_unique<binary_operator_node>(std::move(lhs_node),
                                                                           op_token,
                                                                           std::move(rhs_node));

                const auto tokens = tokens_container_t{ lhs_token, op_token, rhs_lhs_token, rhs_op_token, rhs_rhs_token };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.expr();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Expr_IdOpenParenCloseParen_GetFunctionCallWithoutParameters)
            {
                // foo()
                const auto fun_name_token = token_identifier("foo");

                auto expected_ast = std::make_unique<function_call_node>(fun_name_token, function_call_node::params_t{});

                const auto tokens = tokens_container_t{ fun_name_token, token_open_paren(), token_close_paren() };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.expr();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Expr_IdOpenParenParametersCloseParen_GetFunctionCallWithParameters)
            {
                // foo(bar, "baz", 42)
                const auto fun_name_token = token_identifier("foo");
                const auto param1 = token_identifier("bar");
                const auto param2 = token_string("baz");
                const auto param3 = token_integer("42");
                const auto comma = token_comma();

                // Todo: initialize instead of emplaces
                function_call_node::params_t params;
                params.emplace_back(std::make_unique<id_node>(param1));
                params.emplace_back(std::make_unique<string_value_node>(param2));
                params.emplace_back(std::make_unique<int_value_node>(param3));

                auto expected_ast = std::make_unique<function_call_node>(fun_name_token, std::move(params));

                const auto tokens = tokens_container_t{ fun_name_token,
                                                        token_open_paren(),
                                                        param1,
                                                        comma,
                                                        param2,
                                                        comma,
                                                        param3,
                                                        token_close_paren() };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.expr();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Expr_IdDotIdOpenParenCloseParen_GetMemberFunctionCallWithoutParameters)
            {
                // foo.bar()
                const auto class_name_token = token_identifier("foo");
                const auto fun_name_token = token_identifier("bar");

                auto lhs = std::make_unique<id_node>(class_name_token);

                auto expected_ast = std::make_unique<member_function_call_node>(std::move(lhs),
                                                                                fun_name_token,
                                                                                function_call_node::params_t{});

                const auto tokens = tokens_container_t{class_name_token,
                                                       token_dot(),
                                                       fun_name_token,
                                                       token_open_paren(),
                                                       token_close_paren() };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.expr();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Expr_IdDotIdOpenParenParametersCloseParen_GetMemberFunctionCallWithParameters)
            {
                // foo.bar(baz, "qux", 42)
                const auto class_name_token = token_identifier("foo");
                const auto fun_name_token = token_identifier("bar");
                const auto param1 = token_identifier("baz");
                const auto param2 = token_string("qux");
                const auto param3 = token_integer("42");
                const auto comma = token_comma();

                auto lhs = std::make_unique<id_node>(class_name_token);

                // Todo: initialize instead of emplaces
                function_call_node::params_t params;
                params.emplace_back(std::make_unique<id_node>(param1));
                params.emplace_back(std::make_unique<string_value_node>(param2));
                params.emplace_back(std::make_unique<int_value_node>(param3));

                auto expected_ast = std::make_unique<member_function_call_node>(std::move(lhs),
                                                                                fun_name_token,
                                                                                std::move(params));

                const auto tokens = tokens_container_t{ class_name_token,
                                                        token_dot(),
                                                        fun_name_token,
                                                        token_open_paren(),
                                                        param1,
                                                        comma,
                                                        param2,
                                                        comma,
                                                        param3,
                                                        token_close_paren() };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.expr();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Expr_ParenthesizedExpressionDotIdOpenParenCloseParen_GetMemberFunctionCallWithoutParameters)
            {
                // (foo+bar).baz()
                const auto lhs_lhs_token = token_identifier("foo");
                const auto lhs_op_token = token_plus();
                const auto lhs_rhs_token = token_identifier("bar");
                const auto fun_name_token = token_identifier("baz");

                auto lhs_lhs_node = std::make_unique<id_node>(lhs_lhs_token);
                auto lhs_rhs_node = std::make_unique<id_node>(lhs_rhs_token);
                auto lhs_node = std::make_unique<binary_operator_node>(std::move(lhs_lhs_node),
                                                                       lhs_op_token,
                                                                       std::move(lhs_rhs_node));

                auto expected_ast = std::make_unique<member_function_call_node>(std::move(lhs_node),
                                                                                fun_name_token,
                                                                                function_call_node::params_t{});

                const auto tokens = tokens_container_t{token_open_paren(),
                                                       lhs_lhs_token,
                                                       lhs_op_token,
                                                       lhs_rhs_token,
                                                       token_close_paren(),
                                                       token_dot(),
                                                       fun_name_token,
                                                       token_open_paren(),
                                                       token_close_paren() };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.expr();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Expr_ParenthesizedExpressionDotIdOpenParenParamsCloseParen_GetMemberFunctionCallWithParameters)
            {
                // (foo+bar).baz(baz, "qux", 42)
                const auto lhs_lhs_token = token_identifier("foo");
                const auto lhs_op_token = token_plus();
                const auto lhs_rhs_token = token_identifier("bar");
                const auto fun_name_token = token_identifier("baz");
                const auto param1 = token_identifier("baz");
                const auto param2 = token_string("qux");
                const auto param3 = token_integer("42");
                const auto comma = token_comma();

                auto lhs_lhs_node = std::make_unique<id_node>(lhs_lhs_token);
                auto lhs_rhs_node = std::make_unique<id_node>(lhs_rhs_token);
                auto lhs_node = std::make_unique<binary_operator_node>(std::move(lhs_lhs_node),
                                                                       lhs_op_token,
                                                                       std::move(lhs_rhs_node));
                function_call_node::params_t params;
                params.emplace_back(std::make_unique<id_node>(param1));
                params.emplace_back(std::make_unique<string_value_node>(param2));
                params.emplace_back(std::make_unique<int_value_node>(param3));

                auto expected_ast = std::make_unique<member_function_call_node>(std::move(lhs_node),
                                                                                fun_name_token,
                                                                                std::move(params));

                const auto tokens = tokens_container_t{token_open_paren(),
                                                       lhs_lhs_token,
                                                       lhs_op_token,
                                                       lhs_rhs_token,
                                                       token_close_paren(),
                                                       token_dot(),
                                                       fun_name_token,
                                                       token_open_paren(),
                                                       param1,
                                                       comma,
                                                       param2,
                                                       comma,
                                                       param3,
                                                       token_close_paren() };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.expr();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Expr_IdDotId_GetClassMemberAccess)
            {
                // foo.bar
                const auto class_name_token = token_identifier("foo");
                const auto member_name_token = token_identifier("bar");

                auto lhs_node = std::make_unique<id_node>(class_name_token);

                auto expected_ast = std::make_unique<class_member_access_node>(std::move(lhs_node), member_name_token);

                const auto tokens = tokens_container_t{class_name_token, token_dot(), member_name_token };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.expr();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Expr_ParenthesizedExpressionDotId_GetClassMemberAccess)
            {
                // (foo + bar).baz
                const auto lhs_lhs_token = token_identifier("foo");
                const auto lhs_op_token = token_plus();
                const auto lhs_rhs_token = token_identifier("bar");
                const auto member_name_token = token_identifier("baz");

                auto lhs_lhs_node = std::make_unique<id_node>(lhs_lhs_token);
                auto lhs_rhs_node = std::make_unique<id_node>(lhs_rhs_token);
                auto lhs_node = std::make_unique<binary_operator_node>(std::move(lhs_lhs_node),
                                                                       lhs_op_token,
                                                                       std::move(lhs_rhs_node));

                auto expected_ast = std::make_unique<class_member_access_node>(std::move(lhs_node), member_name_token);

                const auto tokens = tokens_container_t{token_open_paren(),
                                                       lhs_lhs_token,
                                                       lhs_op_token,
                                                       lhs_rhs_token,
                                                       token_close_paren(),
                                                       token_dot(),
                                                       member_name_token };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.expr();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, If_IfConditionBlock_GetIfElse)
            {
                // if(foo) {}
                const auto condition_token = token_identifier("foo");

                auto condition_expr_node = std::make_unique<id_node>(condition_token);
                auto block = std::make_unique<block_node>(block_node::expressions_t{});
                auto condition_node = std::make_unique<conditional_node>(std::move(condition_expr_node), std::move(block));

                if_else_node::ifs_t ifs;
                ifs.emplace_back(std::move(condition_node));

                auto expected_ast = std::make_unique<if_else_node>(std::move(ifs), nullptr);

                const auto tokens = tokens_container_t{token_kw_if(),
                                                       token_open_paren(),
                                                       condition_token,
                                                       token_close_paren(),
                                                       token_open_brace(),
                                                       token_close_brace() };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.get_if_else_node();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, If_IfConditionParenBlockElseBlock_GetIfElse)
            {
                // if(foo) {} else {}
                const auto condition_token = token_identifier("foo");

                auto condition_expr_node = std::make_unique<id_node>(condition_token);
                auto block = std::make_unique<block_node>(block_node::expressions_t{});
                auto condition_node = std::make_unique<conditional_node>(std::move(condition_expr_node), std::move(block));

                if_else_node::ifs_t ifs;
                ifs.emplace_back(std::move(condition_node));

                auto else_node = std::make_unique<block_node>(block_node::expressions_t{});

                auto expected_ast = std::make_unique<if_else_node>(std::move(ifs), std::move(else_node));

                const auto tokens = tokens_container_t{ token_kw_if(),
                                                        token_open_paren(),
                                                        condition_token,
                                                        token_close_paren(),
                                                        token_open_brace(),
                                                        token_close_brace(),
                                                        token_kw_else(),
                                                        token_open_brace(),
                                                        token_close_brace()};
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.get_if_else_node();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, If_IfConditionBlockElseIfConditionBlockElseBlock_GetIfElse)
            {
                // if(foo) {} else if(bar) else {}
                const auto if_condition_token = token_identifier("foo");
                const auto elseif_condition_token = token_identifier("bar");

                auto if_condition_expr_node = std::make_unique<id_node>(if_condition_token);
                auto if_block = std::make_unique<block_node>(block_node::expressions_t{});
                auto if_condition_node = std::make_unique<conditional_node>(std::move(if_condition_expr_node), std::move(if_block));

                auto elseif_condition_expr_node = std::make_unique<id_node>(elseif_condition_token);
                auto elseif_block = std::make_unique<block_node>(block_node::expressions_t{});
                auto elseif_condition_node = std::make_unique<conditional_node>(std::move(elseif_condition_expr_node), std::move(elseif_block));

                if_else_node::ifs_t ifs;
                ifs.emplace_back(std::move(if_condition_node));
                ifs.emplace_back(std::move(elseif_condition_node));

                auto else_node = std::make_unique<block_node>(block_node::expressions_t{});

                auto expected_ast = std::make_unique<if_else_node>(std::move(ifs), std::move(else_node));

                const auto tokens = tokens_container_t{ token_kw_if(),
                                                        token_open_paren(),
                                                        if_condition_token,
                                                        token_close_paren(),
                                                        token_open_brace(),
                                                        token_close_brace(),

                                                        token_kw_else(),
                                                        token_kw_if(),
                                                        token_open_paren(),
                                                        elseif_condition_token,
                                                        token_close_paren(),
                                                        token_open_brace(),
                                                        token_close_brace(),

                                                        token_kw_else(),
                                                        token_open_brace(),
                                                        token_close_brace() };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.get_if_else_node();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, If_IfConditionBlockElseIfConditionBlock_GetIfElse)
            {
                // if(foo) {} else if(bar)
                const auto if_condition_token = token_identifier("foo");
                const auto elseif_condition_token = token_identifier("bar");

                auto if_condition_expr_node = std::make_unique<id_node>(if_condition_token);
                auto if_block = std::make_unique<block_node>(block_node::expressions_t{});
                auto if_condition_node = std::make_unique<conditional_node>(std::move(if_condition_expr_node), std::move(if_block));

                auto elseif_condition_expr_node = std::make_unique<id_node>(elseif_condition_token);
                auto elseif_block = std::make_unique<block_node>(block_node::expressions_t{});
                auto elseif_condition_node = std::make_unique<conditional_node>(std::move(elseif_condition_expr_node), std::move(elseif_block));

                if_else_node::ifs_t ifs;
                ifs.emplace_back(std::move(if_condition_node));
                ifs.emplace_back(std::move(elseif_condition_node));

                auto expected_ast = std::make_unique<if_else_node>(std::move(ifs), nullptr);

                const auto tokens = tokens_container_t{ token_kw_if(),
                                                        token_open_paren(),
                                                        if_condition_token,
                                                        token_close_paren(),
                                                        token_open_brace(),
                                                        token_close_brace(),

                                                        token_kw_else(),
                                                        token_kw_if(),
                                                        token_open_paren(),
                                                        elseif_condition_token,
                                                        token_close_paren(),
                                                        token_open_brace(),
                                                        token_close_brace() };
                auto parser = parser_t{  dummy_err_observer, tokens };
                auto result_ast = parser.get_if_else_node();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, While_WhileConditionBlock_GetWhile)
            {
                // while(foo) {}
                const auto condition_token = token_identifier("foo");

                auto condition_expr_node = std::make_unique<id_node>(condition_token);
                auto block = std::make_unique<block_node>(block_node::expressions_t{});
                auto condition_node = std::make_unique<conditional_node>(std::move(condition_expr_node), std::move(block));

                auto expected_ast = std::make_unique<while_node>(std::move(condition_node));

                const auto tokens = tokens_container_t{ token_kw_while(),
                                                        token_open_paren(),
                                                        condition_token,
                                                        token_close_paren(),
                                                        token_open_brace(),
                                                        token_close_brace() };
                auto parser = parser_t{ dummy_err_observer, tokens };
                auto result_ast = parser.get_while_node();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, Return_ReturnExpression_GetReturn)
            {
                // return foo
                const auto expression_token = token_identifier("foo");

                auto expr_node = std::make_unique<id_node>(expression_token);
                auto expected_ast = std::make_unique<return_node>(std::move(expr_node));

                const auto tokens = tokens_container_t{ token_kw_return(),
                                                        expression_token };
                auto parser = parser_t{ dummy_err_observer, tokens };
                auto result_ast = parser.get_return_node();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            namespace type
            {
                using Type_SingleToken = ::testing::TestWithParam<token_t>;

                TEST_P(Type_SingleToken, GetTypeReference)
                {
                    const auto token = token_kw_int();

                    const auto expected_reference = type_reference{ token, token };

                    const auto tokens = tokens_container_t{ token };
                    auto parser = parser_t{ dummy_err_observer, tokens };
                    auto result_type_reference = parser.type();

                    ASSERT_TRUE(result_type_reference);
                    EXPECT_THAT(result_type_reference->begin, Eq(expected_reference.begin));
                    EXPECT_THAT(result_type_reference->end, Eq(expected_reference.end));
                }

                const auto values = ::testing::Values(token_kw_bool(), token_kw_int(), token_kw_double(), token_kw_string(),
                                                     token_identifier("foo"), token_identifier("project"));
                INSTANTIATE_TEST_CASE_P(Parser2Test, Type_SingleToken, values);
            }

            TEST(Parser2Test, Type_GenericType_GetTypeReference)
            {
                cmsl::string_view source = "list<int>";
                const auto list_token = token_from_larger_source(source, token_type_t::kw_list, 0u, 4u);
                const auto less_token = token_from_larger_source(source, token_type_t::less, 4u, 5u);
                const auto int_token = token_from_larger_source(source, token_type_t::kw_int, 5u, 8u);
                const auto greater_token = token_from_larger_source(source, token_type_t::greater, 8u, 9u);

                const auto expected_reference = type_reference{ list_token, greater_token };

                const auto tokens = tokens_container_t{ list_token,
                                                        less_token,
                                                        int_token,
                                                        greater_token };
                auto parser = parser_t{ dummy_err_observer, tokens };
                auto result_type_reference = parser.type();

                ASSERT_TRUE(result_type_reference);
                EXPECT_THAT(result_type_reference->begin, Eq(expected_reference.begin));
                EXPECT_THAT(result_type_reference->end, Eq(expected_reference.end));
            }

            TEST(Parser2Test, Type_NestedGenericType_GetTypeReference)
            {
                cmsl::string_view source = "list<list<foo>>";
                const auto list_token = token_from_larger_source(source, token_type_t::kw_list, 0u, 4u);
                const auto less_token = token_from_larger_source(source, token_type_t::less, 4u, 5u);
                const auto nested_list_token = token_from_larger_source(source, token_type_t::kw_list, 5u, 9u);
                const auto nested_less_token = token_from_larger_source(source, token_type_t::less, 9u, 10u);
                const auto foo_token = token_from_larger_source(source, token_type_t::identifier, 10u, 13u);
                const auto nested_greater_token = token_from_larger_source(source, token_type_t::greater, 13u, 14u);
                const auto greater_token = token_from_larger_source(source, token_type_t::greater, 14u, 15u);

                const auto expected_reference = type_reference{ list_token, greater_token };

                const auto tokens = tokens_container_t{ list_token,
                                                        less_token,
                                                        nested_list_token,
                                                        nested_less_token,
                                                        foo_token,
                                                        nested_greater_token,
                                                        greater_token };
                auto parser = parser_t{ dummy_err_observer, tokens };
                auto result_type_reference = parser.type();

                ASSERT_TRUE(result_type_reference);
                EXPECT_THAT(result_type_reference->begin, Eq(expected_reference.begin));
                EXPECT_THAT(result_type_reference->end, Eq(expected_reference.end));
            }

            TEST(Parser2Test, VariableDeclaration_TypeId_GetVariableDeclaration)
            {
                // int foo;
                const auto type_token = token_kw_int();
                const auto name_token = token_identifier("foo");
                const auto semicolon_token = token_semicolon();

                const auto type_ref = type_reference{ type_token, type_token };

                auto expected_ast = std::make_unique<variable_declaration_node>(type_ref, name_token, nullptr);

                const auto tokens = tokens_container_t{ type_token,
                                                        name_token,
                                                        token_semicolon() };
                auto parser = parser_t{ dummy_err_observer, tokens };
                auto result_ast = parser.variable_declaration();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, VariableDeclaration_GenericTypeId_GetVariableDeclaration)
            {
                // int foo;
                cmsl::string_view source = "list<int>";
                const auto list_token = token_from_larger_source(source, token_type_t::kw_list, 0u, 4u);
                const auto less_token = token_from_larger_source(source, token_type_t::less, 4u, 5u);
                const auto int_token = token_from_larger_source(source, token_type_t::kw_int, 5u, 8u);
                const auto greater_token = token_from_larger_source(source, token_type_t::greater, 8u, 9u);
                const auto name_token = token_identifier("foo");

                const auto type_ref = type_reference{ list_token, greater_token };

                auto expected_ast = std::make_unique<variable_declaration_node>(type_ref, name_token, nullptr);

                const auto tokens = tokens_container_t{ list_token,
                                                        less_token,
                                                        int_token,
                                                        greater_token,
                                                        name_token,
                                                        token_semicolon() };
                auto parser = parser_t{ dummy_err_observer, tokens };
                auto result_ast = parser.variable_declaration();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }

            TEST(Parser2Test, VariableDeclaration_TypeIdEqualExpression_GetVariableDeclaration)
            {
                // int foo;
                const auto type_token = token_kw_int();
                const auto name_token = token_identifier("foo");
                const auto int_expr_token = token_integer("42");

                const auto type_ref = type_reference{ type_token, type_token };

                auto expr = std::make_unique<int_value_node>(int_expr_token);
                auto expected_ast = std::make_unique<variable_declaration_node>(type_ref, name_token, std::move(expr));

                const auto tokens = tokens_container_t{ type_token,
                                                        name_token,
                                                        token_equal(),
                                                        int_expr_token,
                                                        token_semicolon() };
                auto parser = parser_t{ dummy_err_observer, tokens };
                auto result_ast = parser.variable_declaration();

                ASSERT_THAT(result_ast, NotNull());
                EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
            }
        }
    }
}
