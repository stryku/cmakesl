#include "ast/ast_node_visitor.hpp"
#include "ast/block_node.hpp"
#include "ast/class_node.hpp"
#include "ast/conditional_node.hpp"
#include "ast/if_else_node.hpp"
#include "ast/infix_nodes.hpp"
#include "ast/parser.hpp"
#include "ast/return_node.hpp"
#include "ast/translation_unit_node.hpp"
#include "ast/user_function_node.hpp"
#include "ast/variable_declaration_node.hpp"
#include "ast/while_node.hpp"

#include "errors/errors_observer.hpp"

#include "test/common/tokens.hpp"

#include <gmock/gmock.h>

namespace cmsl::ast::test {
using token_t = cmsl::lexer::token;
using token_type_t = cmsl::lexer::token_type;
using tokens_container_t = cmsl::lexer::token_container_t;
using parser_t = cmsl::ast::parser;

using namespace cmsl::test::common;
errors::errors_observer dummy_err_observer;

const auto tmp_token = token_identifier("");

using ::testing::NotNull;
using ::testing::Ref;
using ::testing::Eq;

class ast_tree_representation_visitor : public ast_node_visitor
{
public:
  virtual void visit(const block_node& node) override
  {
    m_result += "block{";
    for (const auto& n : node.nodes()) {
      n->visit(*this);
    }
    m_result += "}";
  }

  virtual void visit(const class_node& node) override
  {
    m_result += "class{name:" + std::string{ node.name().str() } + ";members:";

    for (const auto& n : node.nodes()) {
      n->visit(*this);
    }

    m_result += "}";
  }

  virtual void visit(const conditional_node& node) override
  {
    m_result += "conditional{condition:";
    node.condition().visit(*this);
    m_result += ";block:";
    node.body().visit(*this);
    m_result += "}";
  }

  virtual void visit(const if_else_node& node) override
  {
    m_result += "if_else{ifs:";
    for (const auto& if_ : node.ifs()) {
      if_.conditional->visit(*this);
    }
    m_result += ";else:";
    auto else_node = node.else_body();
    if (else_node) {
      else_node->visit(*this);
    }
    m_result += "}";
  }

  virtual void visit(const binary_operator_node& node) override
  {
    m_result += "binary_operator{lhs:";
    node.lhs().visit(*this);
    m_result += ";operator:" + std::string{ node.operator_().str() } + ";rhs:";
    node.rhs().visit(*this);
    m_result += "}";
  }

  virtual void visit(const class_member_access_node& node) override
  {
    m_result += "class_member_access{lhs:";
    node.lhs().visit(*this);
    m_result +=
      ";member_name:" + std::string{ node.member_name().str() } + "}";
  }

  virtual void visit(const function_call_node& node) override
  {
    m_result +=
      "function_call{name:" + std::string{ node.name().str() } + ";params:";
    for (const auto& param : node.param_nodes()) {
      param->visit(*this);
    }
    m_result += "}";
  }

  virtual void visit(const member_function_call_node& node) override
  {
    m_result +=
      "function_call{name:" + std::string{ node.name().str() } + ";params:";
    m_result += "function_call{lhs:";
    node.lhs().visit(*this);
    m_result += ";name:" + std::string{ node.name().str() } + ";params:";
    for (const auto& param : node.param_nodes()) {
      param->visit(*this);
    }
    m_result += "}";
  }

  virtual void visit(const bool_value_node& node) override
  {
    m_result += "bool_value{" + std::string{ node.token().str() } + "}";
  }

  virtual void visit(const int_value_node& node) override
  {
    m_result += "int_value{" + std::string{ node.token().str() } + "}";
  }

  virtual void visit(const double_value_node& node) override
  {
    m_result += "double_value{" + std::string{ node.token().str() } + "}";
  }

  virtual void visit(const string_value_node& node) override
  {
    m_result += "string_value{" + std::string{ node.token().str() } + "}";
  }

  virtual void visit(const id_node& node) override
  {
    m_result += "id{" + std::string{ node.get_identifier().str() } + "}";
  }

  virtual void visit(const return_node& node) override
  {
    m_result += "return{";
    node.expression().visit(*this);
    m_result += "}";
  }

  virtual void visit(const translation_unit_node& node) override
  {
    m_result += "translation_unit{";
    for (const auto& n : node.nodes()) {
      n->visit(*this);
    }
    m_result += "}";
  }

  virtual void visit(const user_function_node& node) override
  {
    m_result += "user_function{return_type:";
    const auto ret_type_reference = node.return_type_representation();
    m_result += ret_type_reference.to_string() + ";name:";
    m_result += std::string{ node.name().str() } + ";params:";
    for (const auto& param_declaration : node.param_declarations()) {
      m_result += "param_declaration{type:" + param_declaration.ty.to_string();
      m_result += ";name:" + std::string{ param_declaration.name.str() } + "}";
    }
    m_result += ";body:";
    node.body().visit(*this);
    m_result += "}";
  }

  virtual void visit(const variable_declaration_node& node) override
  {
    m_result += "variable_declaration{type:";
    const auto ret_type_reference = node.type();
    m_result += ret_type_reference.to_string() + ";name:";
    m_result += std::string{ node.name().str() } + ";initialization:";
    if (const auto init = node.initialization()) {
      init->visit(*this);
    }
    m_result += "}";
  }

  virtual void visit(const while_node& node) override
  {
    m_result += "while{";
    node.node().visit(*this);
    m_result += "}";
  }

  virtual void visit(const initializer_list_node& node) override
  {
    m_result += "initializer_list_node{";
    std::string separator = "";
    for (const auto& value : node.values()) {
      m_result += separator;
      value->visit(*this);
      separator = ';';
    }
    m_result += "}";
  }

  std::string result() const { return m_result; }

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
  *result_listener << "result: " << result_visitor.result()
                   << "\nexpected: " << expected_visitor.result();

  return result_visitor.result() == expected_visitor.result();
}

TEST(Parser2Test, Factor_TrueKeywordToken_GetBoolFundamentalValue)
{
  const auto token = token_kw_true();

  auto expected_ast = std::make_unique<bool_value_node>(token);

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_factor();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Factor_FalseKeywordToken_GetBoolFundamentalValue)
{
  const auto token = token_kw_false();

  auto expected_ast = std::make_unique<bool_value_node>(token);

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_factor();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Factor_IntegerToken_GetIntFundamentalValue)
{
  const auto token = token_integer("1");

  auto expected_ast = std::make_unique<int_value_node>(token);

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_factor();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Factor_DoubleToken_GetDoubleFundamentalValue)
{
  const auto token = token_double("1.6");

  auto expected_ast = std::make_unique<double_value_node>(token);

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_factor();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Factor_StringToken_GetStringFundamentalValue)
{
  const auto token = token_string("some string");

  auto expected_ast = std::make_unique<string_value_node>(token);

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_factor();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Factor_IdentifierToken_GetId)
{
  const auto token = token_identifier("some_name");

  auto expected_ast = std::make_unique<id_node>(token);

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_factor();

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
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_factor();

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
  auto expected_ast = std::make_unique<binary_operator_node>(
    std::move(lhs_node), op_token, std::move(rhs_node));

  const auto tokens = tokens_container_t{ lhs_token, op_token, rhs_token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

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
  auto lhs_node = std::make_unique<binary_operator_node>(
    std::move(lhs_lhs_node), lhs_op_token, std::move(lhs_rhs_node));

  auto rhs_node = std::make_unique<double_value_node>(rhs_token);

  auto expected_ast = std::make_unique<binary_operator_node>(
    std::move(lhs_node), op_token, std::move(rhs_node));

  const auto tokens = tokens_container_t{ lhs_lhs_token, lhs_op_token,
                                          lhs_rhs_token, op_token, rhs_token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

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
  auto rhs_node = std::make_unique<binary_operator_node>(
    std::move(rhs_lhs_node), rhs_op_token, std::move(rhs_rhs_node));

  auto expected_ast = std::make_unique<binary_operator_node>(
    std::move(lhs_node), op_token, std::move(rhs_node));

  const auto tokens = tokens_container_t{ lhs_token, op_token, rhs_lhs_token,
                                          rhs_op_token, rhs_rhs_token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Expr_IdOpenParenCloseParen_GetFunctionCallWithoutParameters)
{
  // foo()
  const auto fun_name_token = token_identifier("foo");

  auto expected_ast = std::make_unique<function_call_node>(
    fun_name_token, tmp_token, function_call_node::params_t{}, tmp_token);

  const auto tokens = tokens_container_t{ fun_name_token, token_open_paren(),
                                          token_close_paren() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test,
     Expr_IdOpenParenParametersCloseParen_GetFunctionCallWithParameters)
{
  // foo(bar, "baz", 42)
  const auto fun_name_token = token_identifier("foo");
  const auto param1 = token_identifier("bar");
  const auto param2 = token_string("baz");
  const auto param3 = token_integer("42");
  const auto comma = token_comma();

  function_call_node::params_t params;
  params.emplace_back(std::make_unique<id_node>(param1));
  params.emplace_back(std::make_unique<string_value_node>(param2));
  params.emplace_back(std::make_unique<int_value_node>(param3));

  auto expected_ast = std::make_unique<function_call_node>(
    fun_name_token, tmp_token, std::move(params), tmp_token);

  const auto tokens = tokens_container_t{
    fun_name_token, token_open_paren(), param1, comma, param2, comma,
    param3,         token_close_paren()
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test,
     Expr_IdDotIdOpenParenCloseParen_GetMemberFunctionCallWithoutParameters)
{
  // foo.bar()
  const auto class_name_token = token_identifier("foo");
  const auto fun_name_token = token_identifier("bar");

  auto lhs = std::make_unique<id_node>(class_name_token);

  auto expected_ast = std::make_unique<member_function_call_node>(
    std::move(lhs), tmp_token, fun_name_token, tmp_token,
    function_call_node::params_t{}, tmp_token);

  const auto tokens =
    tokens_container_t{ class_name_token, token_dot(), fun_name_token,
                        token_open_paren(), token_close_paren() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(
  Parser2Test,
  Expr_IdDotIdOpenParenParametersCloseParen_GetMemberFunctionCallWithParameters)
{
  // foo.bar(baz, "qux", 42)
  const auto class_name_token = token_identifier("foo");
  const auto fun_name_token = token_identifier("bar");
  const auto param1 = token_identifier("baz");
  const auto param2 = token_string("qux");
  const auto param3 = token_integer("42");
  const auto comma = token_comma();

  auto lhs = std::make_unique<id_node>(class_name_token);

  function_call_node::params_t params;
  params.emplace_back(std::make_unique<id_node>(param1));
  params.emplace_back(std::make_unique<string_value_node>(param2));
  params.emplace_back(std::make_unique<int_value_node>(param3));

  auto expected_ast = std::make_unique<member_function_call_node>(
    std::move(lhs), tmp_token, fun_name_token, tmp_token, std::move(params),
    tmp_token);

  const auto tokens =
    tokens_container_t{ class_name_token, token_dot(),
                        fun_name_token,   token_open_paren(),
                        param1,           comma,
                        param2,           comma,
                        param3,           token_close_paren() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(
  Parser2Test,
  Expr_ParenthesizedExpressionDotIdOpenParenCloseParen_GetMemberFunctionCallWithoutParameters)
{
  // (foo+bar).baz()
  const auto lhs_lhs_token = token_identifier("foo");
  const auto lhs_op_token = token_plus();
  const auto lhs_rhs_token = token_identifier("bar");
  const auto fun_name_token = token_identifier("baz");

  auto lhs_lhs_node = std::make_unique<id_node>(lhs_lhs_token);
  auto lhs_rhs_node = std::make_unique<id_node>(lhs_rhs_token);
  auto lhs_node = std::make_unique<binary_operator_node>(
    std::move(lhs_lhs_node), lhs_op_token, std::move(lhs_rhs_node));

  auto expected_ast = std::make_unique<member_function_call_node>(
    std::move(lhs_node), tmp_token, fun_name_token, tmp_token,
    function_call_node::params_t{}, tmp_token);

  const auto tokens = tokens_container_t{
    token_open_paren(), lhs_lhs_token,       lhs_op_token,
    lhs_rhs_token,      token_close_paren(), token_dot(),
    fun_name_token,     token_open_paren(),  token_close_paren()
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(
  Parser2Test,
  Expr_ParenthesizedExpressionDotIdOpenParenParamsCloseParen_GetMemberFunctionCallWithParameters)
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
  auto lhs_node = std::make_unique<binary_operator_node>(
    std::move(lhs_lhs_node), lhs_op_token, std::move(lhs_rhs_node));
  function_call_node::params_t params;
  params.emplace_back(std::make_unique<id_node>(param1));
  params.emplace_back(std::make_unique<string_value_node>(param2));
  params.emplace_back(std::make_unique<int_value_node>(param3));

  auto expected_ast = std::make_unique<member_function_call_node>(
    std::move(lhs_node), tmp_token, fun_name_token, tmp_token,
    std::move(params), tmp_token);

  const auto tokens = tokens_container_t{ token_open_paren(),
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
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Expr_IdDotId_GetClassMemberAccess)
{
  // foo.bar
  const auto class_name_token = token_identifier("foo");
  const auto member_name_token = token_identifier("bar");

  auto lhs_node = std::make_unique<id_node>(class_name_token);

  auto expected_ast = std::make_unique<class_member_access_node>(
    std::move(lhs_node), tmp_token, member_name_token);

  const auto tokens =
    tokens_container_t{ class_name_token, token_dot(), member_name_token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

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
  auto lhs_node = std::make_unique<binary_operator_node>(
    std::move(lhs_lhs_node), lhs_op_token, std::move(lhs_rhs_node));

  auto expected_ast = std::make_unique<class_member_access_node>(
    std::move(lhs_node), tmp_token, member_name_token);

  const auto tokens =
    tokens_container_t{ token_open_paren(), lhs_lhs_token,       lhs_op_token,
                        lhs_rhs_token,      token_close_paren(), token_dot(),
                        member_name_token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, If_IfConditionBlock_GetIfElse)
{
  // if(foo) {}
  const auto condition_token = token_identifier("foo");

  auto condition_expr_node = std::make_unique<id_node>(condition_token);
  auto block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(condition_expr_node), tmp_token, std::move(block));

  if_else_node::ifs_t ifs;
  ifs.emplace_back(if_else_node::if_values{ std::nullopt, tmp_token,
                                            std::move(condition_node) });

  auto expected_ast =
    std::make_unique<if_else_node>(std::move(ifs), std::nullopt);

  const auto tokens =
    tokens_container_t{ token_kw_if(),      token_open_paren(),
                        condition_token,    token_close_paren(),
                        token_open_brace(), token_close_brace() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_if_else_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, If_IfConditionParenBlockElseBlock_GetIfElse)
{
  // if(foo) {} else {}
  const auto condition_token = token_identifier("foo");

  auto condition_expr_node = std::make_unique<id_node>(condition_token);
  auto block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(condition_expr_node), tmp_token, std::move(block));

  if_else_node::ifs_t ifs;
  ifs.emplace_back(if_else_node::if_values{ std::nullopt, tmp_token,
                                            std::move(condition_node) });

  auto else_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto else_value =
    if_else_node::last_else_value{ tmp_token, std::move(else_node) };

  auto expected_ast =
    std::make_unique<if_else_node>(std::move(ifs), std::move(else_value));

  const auto tokens = tokens_container_t{
    token_kw_if(),       token_open_paren(), condition_token,
    token_close_paren(), token_open_brace(), token_close_brace(),
    token_kw_else(),     token_open_brace(), token_close_brace()
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_if_else_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, If_IfConditionBlockElseIfConditionBlockElseBlock_GetIfElse)
{
  // if(foo) {} else if(bar) else {}
  const auto if_condition_token = token_identifier("foo");
  const auto elseif_condition_token = token_identifier("bar");

  auto if_condition_expr_node = std::make_unique<id_node>(if_condition_token);
  auto if_block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto if_condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(if_condition_expr_node), tmp_token,
    std::move(if_block));

  auto elseif_condition_expr_node =
    std::make_unique<id_node>(elseif_condition_token);
  auto elseif_block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto elseif_condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(elseif_condition_expr_node), tmp_token,
    std::move(elseif_block));

  if_else_node::ifs_t ifs;
  ifs.emplace_back(if_else_node::if_values{ std::nullopt, tmp_token,
                                            std::move(if_condition_node) });
  ifs.emplace_back(if_else_node::if_values{
    tmp_token, tmp_token, std::move(elseif_condition_node) });

  auto else_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto else_value =
    if_else_node::last_else_value{ tmp_token, std::move(else_node) };

  auto expected_ast =
    std::make_unique<if_else_node>(std::move(ifs), std::move(else_value));

  const auto tokens = tokens_container_t{
    token_kw_if(),          token_open_paren(),  if_condition_token,
    token_close_paren(),    token_open_brace(),  token_close_brace(),

    token_kw_else(),        token_kw_if(),       token_open_paren(),
    elseif_condition_token, token_close_paren(), token_open_brace(),
    token_close_brace(),

    token_kw_else(),        token_open_brace(),  token_close_brace()
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_if_else_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, If_IfConditionBlockElseIfConditionBlock_GetIfElse)
{
  // if(foo) {} else if(bar)
  const auto if_condition_token = token_identifier("foo");
  const auto elseif_condition_token = token_identifier("bar");

  auto if_condition_expr_node = std::make_unique<id_node>(if_condition_token);
  auto if_block = std::make_unique<block_node>(
    tmp_token, block_node::nodes_t{},
    tmp_token); // Todo: introduce a fixture and method to create empty block
                // node
  auto if_condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(if_condition_expr_node), tmp_token,
    std::move(if_block));

  auto elseif_condition_expr_node =
    std::make_unique<id_node>(elseif_condition_token);
  auto elseif_block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto elseif_condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(elseif_condition_expr_node), tmp_token,
    std::move(elseif_block));

  if_else_node::ifs_t ifs;
  ifs.emplace_back(if_else_node::if_values{ std::nullopt, tmp_token,
                                            std::move(if_condition_node) });
  ifs.emplace_back(if_else_node::if_values{
    tmp_token, tmp_token, std::move(elseif_condition_node) });

  auto expected_ast =
    std::make_unique<if_else_node>(std::move(ifs), std::nullopt);

  const auto tokens = tokens_container_t{
    token_kw_if(),          token_open_paren(),  if_condition_token,
    token_close_paren(),    token_open_brace(),  token_close_brace(),

    token_kw_else(),        token_kw_if(),       token_open_paren(),
    elseif_condition_token, token_close_paren(), token_open_brace(),
    token_close_brace()
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_if_else_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, While_WhileConditionBlock_GetWhile)
{
  // while(foo) {}
  const auto condition_token = token_identifier("foo");

  auto condition_expr_node = std::make_unique<id_node>(condition_token);
  auto block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(condition_expr_node), tmp_token, std::move(block));

  auto expected_ast =
    std::make_unique<while_node>(tmp_token, std::move(condition_node));

  const auto tokens =
    tokens_container_t{ token_kw_while(),   token_open_paren(),
                        condition_token,    token_close_paren(),
                        token_open_brace(), token_close_brace() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_while_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Return_ReturnExpression_GetReturn)
{
  // return foo;
  const auto expression_token = token_identifier("foo");

  auto expr_node = std::make_unique<id_node>(expression_token);
  auto expected_ast =
    std::make_unique<return_node>(tmp_token, std::move(expr_node), tmp_token);

  const auto tokens = tokens_container_t{ token_kw_return(), expression_token,
                                          token_semicolon() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_return_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

namespace type {
using Type_SingleToken = ::testing::TestWithParam<token_t>;

TEST_P(Type_SingleToken, GetTypeReference)
{
  const auto token = token_kw_int();

  const auto expected_reference = type_representation{ token };

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_type_reference = parser.parse_type();

  ASSERT_TRUE(result_type_reference);
  EXPECT_THAT(result_type_reference->tokens(),
              Eq(expected_reference.tokens()));
}

const auto values = ::testing::Values(
  token_kw_bool(), token_kw_int(), token_kw_double(), token_kw_string(),
  token_kw_project(), token_kw_library(), token_kw_executable(),
  token_kw_auto(), token_identifier("foo"));
INSTANTIATE_TEST_CASE_P(Parser2Test, Type_SingleToken, values);
}

TEST(Parser2Test, Type_GenericType_GetTypeReference)
{
  cmsl::string_view source = "list<int>";
  const auto list_token =
    token_from_larger_source(source, token_type_t::kw_list, 0u, 4u);
  const auto less_token =
    token_from_larger_source(source, token_type_t::less, 4u, 5u);
  const auto int_token =
    token_from_larger_source(source, token_type_t::kw_int, 5u, 8u);
  const auto greater_token =
    token_from_larger_source(source, token_type_t::greater, 8u, 9u);

  const auto value_type_representation = type_representation{ int_token };
  const auto expected_type_tokens =
    tokens_container_t{ list_token, less_token, int_token, greater_token };
  const auto expected_reference =
    type_representation{ expected_type_tokens, { value_type_representation } };

  auto parser = parser_t{ dummy_err_observer, cmsl::source_view{ "" },
                          expected_type_tokens };
  auto result_type_reference = parser.parse_type();

  ASSERT_TRUE(result_type_reference);
  EXPECT_THAT(*result_type_reference, Eq(expected_reference));
}

TEST(Parser2Test, Type_NestedGenericType_GetTypeReference)
{
  cmsl::string_view source = "list<list<foo>>";
  const auto list_token =
    token_from_larger_source(source, token_type_t::kw_list, 0u, 4u);
  const auto less_token =
    token_from_larger_source(source, token_type_t::less, 4u, 5u);
  const auto nested_list_token =
    token_from_larger_source(source, token_type_t::kw_list, 5u, 9u);
  const auto nested_less_token =
    token_from_larger_source(source, token_type_t::less, 9u, 10u);
  const auto foo_token =
    token_from_larger_source(source, token_type_t::identifier, 10u, 13u);
  const auto nested_greater_token =
    token_from_larger_source(source, token_type_t::greater, 13u, 14u);
  const auto greater_token =
    token_from_larger_source(source, token_type_t::greater, 14u, 15u);

  // foo
  const auto nested_value_type_representation =
    type_representation{ foo_token };

  // list<foo>
  const auto value_type_representation =
    type_representation{ { nested_list_token, nested_less_token, foo_token,
                           nested_greater_token },
                         { nested_value_type_representation } };

  // list<list<foo>>
  const auto expected_type_tokens =
    tokens_container_t{ list_token,        less_token, nested_list_token,
                        nested_less_token, foo_token,  nested_greater_token,
                        greater_token };
  const auto expected_representation =
    type_representation{ expected_type_tokens, { value_type_representation } };

  auto parser = parser_t{ dummy_err_observer, cmsl::source_view{ "" },
                          expected_type_tokens };
  auto result_type_reference = parser.parse_type();

  ASSERT_TRUE(result_type_reference);
  EXPECT_THAT(*result_type_reference, Eq(expected_representation));
}

TEST(Parser2Test, VariableDeclaration_TypeId_GetVariableDeclaration)
{
  // int foo;
  const auto type_token = token_kw_int();
  const auto name_token = token_identifier("foo");

  const auto type_repr = type_representation{ type_token };

  auto expected_ast = std::make_unique<variable_declaration_node>(
    type_repr, name_token, std::nullopt, tmp_token);

  const auto tokens =
    tokens_container_t{ type_token, name_token, token_semicolon() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_variable_declaration();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, VariableDeclaration_GenericTypeId_GetVariableDeclaration)
{
  // list<int> foo;
  cmsl::string_view source = "list<int>";
  const auto list_token =
    token_from_larger_source(source, token_type_t::kw_list, 0u, 4u);
  const auto less_token =
    token_from_larger_source(source, token_type_t::less, 4u, 5u);
  const auto int_token =
    token_from_larger_source(source, token_type_t::kw_int, 5u, 8u);
  const auto greater_token =
    token_from_larger_source(source, token_type_t::greater, 8u, 9u);
  const auto name_token = token_identifier("foo");

  const auto value_type_representation = type_representation{ int_token };
  const auto representation_tokens =
    tokens_container_t{ list_token, less_token, int_token, greater_token };
  const auto representation =
    type_representation{ representation_tokens,
                         { value_type_representation } };

  auto expected_ast = std::make_unique<variable_declaration_node>(
    representation, name_token, std::nullopt, tmp_token);

  const auto tokens =
    tokens_container_t{ list_token,    less_token, int_token,
                        greater_token, name_token, token_semicolon() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_variable_declaration();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test,
     VariableDeclaration_TypeIdEqualExpression_GetVariableDeclaration)
{
  // int foo;
  const auto type_token = token_kw_int();
  const auto name_token = token_identifier("foo");
  const auto int_expr_token = token_integer("42");

  const auto representation = type_representation{ type_token };

  auto expr = std::make_unique<int_value_node>(int_expr_token);
  auto initialization =
    variable_declaration_node::initialization_values{ tmp_token,
                                                      std::move(expr) };
  auto expected_ast = std::make_unique<variable_declaration_node>(
    representation, name_token, std::move(initialization), tmp_token);

  const auto tokens =
    tokens_container_t{ type_token, name_token, token_equal(), int_expr_token,
                        token_semicolon() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_variable_declaration();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test,
     VariableDeclaration_GenericTypeIdEqualExpression_GetVariableDeclaration)
{
  // list<int> foo = 42;
  cmsl::string_view source = "list<int>";
  const auto list_token =
    token_from_larger_source(source, token_type_t::kw_list, 0u, 4u);
  const auto less_token =
    token_from_larger_source(source, token_type_t::less, 4u, 5u);
  const auto int_token =
    token_from_larger_source(source, token_type_t::kw_int, 5u, 8u);
  const auto greater_token =
    token_from_larger_source(source, token_type_t::greater, 8u, 9u);
  const auto name_token = token_identifier("foo");
  const auto int_expr_token = token_integer("42");

  const auto value_type_representation = type_representation{ int_token };
  const auto representation_tokens =
    tokens_container_t{ list_token, less_token, int_token, greater_token };
  const auto representation =
    type_representation{ representation_tokens,
                         { value_type_representation } };

  auto expr = std::make_unique<int_value_node>(int_expr_token);
  auto initialization =
    variable_declaration_node::initialization_values{ tmp_token,
                                                      std::move(expr) };
  auto expected_ast = std::make_unique<variable_declaration_node>(
    representation, name_token, std::move(initialization), tmp_token);

  const auto tokens =
    tokens_container_t{ list_token,     less_token,       int_token,
                        greater_token,  name_token,       token_equal(),
                        int_expr_token, token_semicolon() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_variable_declaration();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Block_OpenBraceCloseBrace_GetBlock)
{
  // {}
  auto expected_ast =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);

  const auto tokens =
    tokens_container_t{ token_open_brace(), token_close_brace() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_block();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Block_VariableDeclaration_GetBlock)
{
  // { foo bar; }
  const auto variable_type_token = token_identifier("foo");
  const auto variable_name_token = token_identifier("bar");
  const auto variable_type_ref = type_representation{ variable_type_token };
  auto variable_decl_node = std::make_unique<variable_declaration_node>(
    variable_type_ref, variable_name_token, std::nullopt, tmp_token);

  block_node::nodes_t exprs;
  exprs.emplace_back(std::move(variable_decl_node));

  auto expected_ast =
    std::make_unique<block_node>(tmp_token, std::move(exprs), tmp_token);

  const auto tokens =
    tokens_container_t{ token_open_brace(), variable_type_token,
                        variable_name_token, token_semicolon(),
                        token_close_brace() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_block();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Block_IfElse_GetBlock)
{
  // { if(foo) {} else if(bar) else {} }
  const auto if_condition_token = token_identifier("foo");
  const auto elseif_condition_token = token_identifier("bar");

  auto if_condition_expr_node = std::make_unique<id_node>(if_condition_token);
  auto if_block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto if_condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(if_condition_expr_node), tmp_token,
    std::move(if_block));

  auto elseif_condition_expr_node =
    std::make_unique<id_node>(elseif_condition_token);
  auto elseif_block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto elseif_condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(elseif_condition_expr_node), tmp_token,
    std::move(elseif_block));

  if_else_node::ifs_t ifs;
  ifs.emplace_back(if_else_node::if_values{ std::nullopt, tmp_token,
                                            std::move(if_condition_node) });
  ifs.emplace_back(if_else_node::if_values{
    tmp_token, tmp_token, std::move(elseif_condition_node) });

  auto else_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto else_value =
    if_else_node::last_else_value{ tmp_token, std::move(else_node) };

  auto if_else =
    std::make_unique<if_else_node>(std::move(ifs), std::move(else_value));

  block_node::nodes_t exprs;
  exprs.emplace_back(std::move(if_else));

  auto expected_ast =
    std::make_unique<block_node>(tmp_token, std::move(exprs), tmp_token);

  const auto tokens = tokens_container_t{
    token_open_brace(),

    token_kw_if(),          token_open_paren(),  if_condition_token,
    token_close_paren(),    token_open_brace(),  token_close_brace(),

    token_kw_else(),        token_kw_if(),       token_open_paren(),
    elseif_condition_token, token_close_paren(), token_open_brace(),
    token_close_brace(),

    token_kw_else(),        token_open_brace(),  token_close_brace(),

    token_close_brace()
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_block();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Block_Expression_GetBlock)
{
  // { foo + bar; }
  const auto lhs_token = token_integer("1");
  const auto op_token = token_plus();
  const auto rhs_token = token_integer("2");

  auto lhs_node = std::make_unique<int_value_node>(lhs_token);
  auto rhs_node = std::make_unique<int_value_node>(rhs_token);
  auto expr_node = std::make_unique<binary_operator_node>(
    std::move(lhs_node), op_token, std::move(rhs_node));

  block_node::nodes_t exprs;
  exprs.emplace_back(std::move(expr_node));

  auto expected_ast =
    std::make_unique<block_node>(tmp_token, std::move(exprs), tmp_token);

  const auto tokens =
    tokens_container_t{ token_open_brace(), lhs_token,
                        op_token,           rhs_token,
                        token_semicolon(),  token_close_brace() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_block();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Block_While_GetBlock)
{
  // { while(foo) {} }
  const auto condition_token = token_identifier("foo");

  auto condition_expr_node = std::make_unique<id_node>(condition_token);
  auto block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(condition_expr_node), tmp_token, std::move(block));

  auto while_ast_node =
    std::make_unique<while_node>(tmp_token, std::move(condition_node));

  block_node::nodes_t exprs;
  exprs.emplace_back(std::move(while_ast_node));

  auto expected_ast =
    std::make_unique<block_node>(tmp_token, std::move(exprs), tmp_token);

  const auto tokens =
    tokens_container_t{ token_open_brace(),  token_kw_while(),
                        token_open_paren(),  condition_token,
                        token_close_paren(), token_open_brace(),
                        token_close_brace(), token_close_brace() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_block();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Block_Return_GetBlock)
{
  // { return foo; }
  const auto expression_token = token_identifier("foo");

  auto expr_node = std::make_unique<id_node>(expression_token);
  auto return_ast_node =
    std::make_unique<return_node>(tmp_token, std::move(expr_node), tmp_token);

  block_node::nodes_t exprs;
  exprs.emplace_back(std::move(return_ast_node));

  auto expected_ast =
    std::make_unique<block_node>(tmp_token, std::move(exprs), tmp_token);

  const auto tokens =
    tokens_container_t{ token_open_brace(), token_kw_return(),
                        expression_token, token_semicolon(),
                        token_close_brace() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_block();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Block_NestedBlock_GetBlock)
{
  // { {} }

  auto nested_block_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  block_node::nodes_t exprs;
  exprs.emplace_back(std::move(nested_block_node));

  auto expected_ast =
    std::make_unique<block_node>(tmp_token, std::move(exprs), tmp_token);

  const auto tokens =
    tokens_container_t{ token_open_brace(), token_open_brace(),
                        token_close_brace(), token_close_brace() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_block();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Function_TypeIdOpenParenCloseParen_GetFunction)
{
  // double foo() {}
  const auto function_type_token = token_kw_double();
  const auto function_name_token = token_identifier("foo");
  const auto function_type_ref = type_representation{ function_type_token };
  auto function_block_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto expected_ast = std::make_unique<user_function_node>(
    function_type_ref, function_name_token, tmp_token,
    user_function_node::params_t{}, tmp_token, std::move(function_block_node));

  const auto tokens =
    tokens_container_t{ function_type_token, function_name_token,
                        token_open_paren(),  token_close_paren(),
                        token_open_brace(),  token_close_brace() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_function();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Function_TypeIdOpenParenParameterCloseParen_GetFunction)
{
  // double foo(bar baz) {}
  const auto function_type_token = token_kw_double();
  const auto function_name_token = token_identifier("foo");
  const auto function_type_ref = type_representation{ function_type_token };

  const auto param_type_token = token_identifier("bar");
  const auto param_name_token = token_identifier("baz");
  const auto param_type_ref = type_representation{ param_type_token };

  user_function_node::params_t params{ param_declaration{ param_type_ref,
                                                          param_name_token } };

  auto function_block_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto expected_ast = std::make_unique<user_function_node>(
    function_type_ref, function_name_token, tmp_token, std::move(params),
    tmp_token, std::move(function_block_node));

  const auto tokens = tokens_container_t{
    function_type_token, function_name_token, token_open_paren(),

    param_type_token,    param_name_token,

    token_close_paren(), token_open_brace(),  token_close_brace()
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_function();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Function_TypeIdOpenParenParametersCloseParen_GetFunction)
{
  // double foo(bar baz, qux out_of_fancy_identifiers) {}
  const auto function_type_token = token_kw_double();
  const auto function_name_token = token_identifier("foo");
  const auto function_type_ref = type_representation{ function_type_token };

  const auto param_type_token = token_identifier("bar");
  const auto param_name_token = token_identifier("baz");
  const auto param_type_ref = type_representation{ param_type_token };

  const auto param2_type_token = token_identifier("qux");
  const auto param2_name_token = token_identifier("out_of_fancy_identifiers");
  const auto param2_type_ref = type_representation{ param2_type_token };

  user_function_node::params_t params{
    param_declaration{ param_type_ref, param_name_token },
    param_declaration{ param2_type_ref, param2_name_token }
  };

  auto function_block_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto expected_ast = std::make_unique<user_function_node>(
    function_type_ref, function_name_token, tmp_token, std::move(params),
    tmp_token, std::move(function_block_node));

  const auto tokens = tokens_container_t{
    function_type_token, function_name_token, token_open_paren(),

    param_type_token,    param_name_token,

    token_comma(),

    param2_type_token,   param2_name_token,

    token_close_paren(), token_open_brace(),  token_close_brace()
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_function();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Class_ClassIdEmptyBlock_GetClass)
{
  // class foo {};
  const auto name_token = token_identifier("foo");

  auto expected_ast =
    std::make_unique<class_node>(tmp_token, name_token, tmp_token,
                                 class_node::nodes_t{}, tmp_token, tmp_token);

  const auto tokens =
    tokens_container_t{ token_kw_class(), name_token, token_open_brace(),
                        token_close_brace(), token_semicolon() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_class();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Class_ClassIdVariableDeclaration_GetClass)
{
  // class foo { int bar; };
  const auto name_token = token_identifier("foo");
  const auto variable_type_token = token_kw_int();
  const auto variable_name_token = token_identifier("bar");
  const auto variable_type_ref = type_representation{ variable_type_token };
  auto variable_decl_node = std::make_unique<variable_declaration_node>(
    variable_type_ref, variable_name_token, std::nullopt, tmp_token);

  class_node::nodes_t nodes;
  nodes.emplace_back(std::move(variable_decl_node));

  auto expected_ast = std::make_unique<class_node>(
    tmp_token, name_token, tmp_token, std::move(nodes), tmp_token, tmp_token);

  const auto tokens =
    tokens_container_t{ token_kw_class(),    name_token,
                        token_open_brace(),  variable_type_token,
                        variable_name_token, token_semicolon(),
                        token_close_brace(), token_semicolon() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_class();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Class_ClassIdFunctionVariableDeclaration_GetClass)
{
  // class foo { double bar() {} baz qux; };
  const auto name_token = token_identifier("foo");

  const auto function_type_token = token_kw_double();
  const auto function_name_token = token_identifier("bar");
  const auto function_type_ref = type_representation{ function_type_token };
  auto function_block_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto fun_node = std::make_unique<user_function_node>(
    function_type_ref, function_name_token, tmp_token,
    user_function_node::params_t{}, tmp_token, std::move(function_block_node));

  const auto variable_type_token = token_identifier("baz");
  const auto variable_name_token = token_identifier("qux");
  const auto variable_type_ref = type_representation{ variable_type_token };
  auto variable_decl_node = std::make_unique<variable_declaration_node>(
    variable_type_ref, variable_name_token, std::nullopt, tmp_token);

  class_node::nodes_t nodes;
  nodes.emplace_back(std::move(fun_node));
  nodes.emplace_back(std::move(variable_decl_node));

  auto expected_ast = std::make_unique<class_node>(
    tmp_token, name_token, tmp_token, std::move(nodes), tmp_token, tmp_token);

  const auto tokens = tokens_container_t{
    token_kw_class(),    name_token,          token_open_brace(),

    function_type_token, function_name_token, token_open_paren(),
    token_close_paren(), token_open_brace(),  token_close_brace(),

    variable_type_token, variable_name_token, token_semicolon(),

    token_close_brace(), token_semicolon()
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_class();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST(Parser2Test, Factor_InitializerList_GetInitializerList)
{
  // { foo, bar(baz, qux), { nest_foo, nest_bar } }
  const auto foo_token = token_identifier("foo");
  const auto bar_token = token_identifier("bar");
  const auto baz_token = token_identifier("baz");
  const auto qux_token = token_identifier("qux");
  const auto nest_foo_token = token_identifier("nest_foo");
  const auto nest_bar_token = token_identifier("nest_bar");
  const auto ob_token = token_open_brace();
  const auto cb_token = token_close_brace();

  auto foo = std::make_unique<id_node>(foo_token);

  auto baz = std::make_unique<id_node>(baz_token);
  auto qux = std::make_unique<id_node>(qux_token);
  std::vector<std::unique_ptr<ast_node>> bar_params;
  bar_params.emplace_back(std::move(baz));
  bar_params.emplace_back(std::move(qux));
  auto bar = std::make_unique<function_call_node>(
    bar_token, tmp_token, std::move(bar_params), tmp_token);

  auto nested_foo = std::make_unique<id_node>(nest_foo_token);
  auto nested_bar = std::make_unique<id_node>(nest_bar_token);
  std::vector<std::unique_ptr<ast_node>> nested_list_values;
  nested_list_values.emplace_back(std::move(nested_foo));
  nested_list_values.emplace_back(std::move(nested_bar));
  auto nested_list = std::make_unique<initializer_list_node>(
    ob_token, std::move(nested_list_values), cb_token);

  std::vector<std::unique_ptr<ast_node>> expected_list_values;
  expected_list_values.emplace_back(std::move(foo));
  expected_list_values.emplace_back(std::move(bar));
  expected_list_values.emplace_back(std::move(nested_list));
  auto expected_ast = std::make_unique<initializer_list_node>(
    ob_token, std::move(expected_list_values), cb_token);

  const auto tokens = tokens_container_t{
    token_open_brace(), foo_token,          token_comma(),
    bar_token,          token_open_paren(), baz_token,
    token_comma(),      qux_token,          token_close_paren(),
    token_comma(),      token_open_brace(), nest_foo_token,
    token_comma(),      nest_bar_token,     token_close_brace(),
    token_close_brace()
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_initializer_list();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}
}
