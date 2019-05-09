#include "sema/sema_builder_ast_visitor.hpp"

#include "ast/block_node.hpp"
#include "ast/class_node.hpp"
#include "ast/conditional_node.hpp"
#include "ast/if_else_node.hpp"
#include "ast/infix_nodes.hpp"
#include "ast/return_node.hpp"
#include "ast/translation_unit_node.hpp"
#include "ast/user_function_node.hpp"
#include "ast/variable_declaration_node.hpp"
#include "ast/while_node.hpp"

#include "errors/errors_observer.hpp"

#include "sema/builtin_token_provider.hpp"
#include "sema/factories.hpp"

#include "test/common/tokens.hpp"
#include "test/errors_observer_mock/errors_observer_mock.hpp"
#include "test/sema/mock/add_subdirectory_semantic_handler_mock.hpp"
#include "test/sema/mock/expression_node_mock.hpp"
#include "test/sema/mock/identifiers_context_mock.hpp"
#include "test/sema/mock/sema_context_mock.hpp"
#include "test/sema/mock/sema_function_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
using ::testing::StrictMock;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::Ref;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Eq;
using ::testing::_;

using namespace cmsl::test::common;

// Todo: to a common file
struct errors_observer_and_mock
{
  StrictMock<errors::test::errors_observer_mock> mock;
  errors::errors_observer observer;
};
using errs_t = errors_observer_and_mock;

const sema_context_impl valid_context;
const sema_type valid_type{ valid_context,
                            ast::type_representation{
                              token_identifier("foo") },
                            {} };

const auto tmp_token = token_identifier("");

class SemaBuilderAstVisitorTest : public ::testing::Test
{
protected:
  sema_type_factory m_type_factory;
  sema_function_factory m_function_factory;
  sema_context_factory m_context_factory;
  add_subdirectory_semantic_handler_mock m_add_subdirectory_mock;
  builtin_token_provider m_token_provider{ "" };

  sema_builder_ast_visitor create_visitor(errs_t& errs, sema_context& ctx,
                                          identifiers_context& ids_ctx)
  {
    return sema_builder_ast_visitor{ ctx,
                                     ctx,
                                     errs.observer,
                                     ids_ctx,
                                     m_type_factory,
                                     m_function_factory,
                                     m_context_factory,
                                     m_add_subdirectory_mock,
                                     m_token_provider };
  }

  ast::function_call_node create_function_call(
    token_t name, ast::function_call_node::params_t params = {})
  {
    return ast::function_call_node{ name, tmp_token, std::move(params),
                                    tmp_token };
  }

  ast::member_function_call_node create_member_function_call_node(
    std::unique_ptr<ast::ast_node> lhs, token_t name,
    ast::member_function_call_node::params_t params = {})
  {
    return ast::member_function_call_node{
      std::move(lhs), tmp_token, name, tmp_token, std::move(params), tmp_token
    };
  }

  std::unique_ptr<ast::block_node> create_block_node_ptr(
    ast::block_node::nodes_t exprs = {})
  {
    return std::make_unique<ast::block_node>(tmp_token, std::move(exprs),
                                             tmp_token);
  }

  std::unique_ptr<ast::variable_declaration_node>
  create_variable_declaration_node(
    ast::type_representation type, token_t name,
    std::optional<ast::variable_declaration_node::initialization_values>
      initialization = std::nullopt)
  {
    return std::make_unique<ast::variable_declaration_node>(
      std::move(type), name, std::move(initialization), tmp_token);
  }

  std::unique_ptr<ast::user_function_node> create_user_function_node(
    ast::type_representation type, token_t name,
    std::unique_ptr<ast::block_node> body,
    ast::user_function_node::params_t params = {})
  {
    return std::make_unique<ast::user_function_node>(
      std::move(type), name, tmp_token, std::move(params), tmp_token,
      std::move(body));
  }
};

MATCHER(IsValidType, "")
{
  return arg == valid_type;
}

MATCHER(ParamDeclarations, "")
{
  return arg == valid_type;
}

TEST_F(SemaBuilderAstVisitorTest, Visit_BoolValue_GetCorrectBoolValue)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  identifiers_context_mock ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto value = true;
  const auto token = token_kw_true();
  ast::bool_value_node node(token);

  EXPECT_CALL(ctx, find_type(_)).WillOnce(Return(&valid_type));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());
  const auto casted_node =
    dynamic_cast<const bool_value_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
  EXPECT_THAT(casted_node->value(), Eq(value));
  EXPECT_THAT(casted_node->type(), IsValidType());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_IntValue_GetCorrectIntValue)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  identifiers_context_mock ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto value = int_t{ 42 };
  const auto token = token_integer("42");
  ast::int_value_node node(token);

  EXPECT_CALL(ctx, find_type(_)).WillOnce(Return(&valid_type));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());
  const auto casted_node =
    dynamic_cast<const int_value_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
  EXPECT_THAT(casted_node->value(), Eq(value));
  EXPECT_THAT(casted_node->type(), IsValidType());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_DoubleValue_GetCorrectDoubleValue)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  identifiers_context_mock ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto value = double{ 4.2 };
  const auto token = token_double("4.2");
  ast::double_value_node node(token);

  EXPECT_CALL(ctx, find_type(_)).WillOnce(Return(&valid_type));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());
  const auto casted_node =
    dynamic_cast<const double_value_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
  EXPECT_NEAR(casted_node->value(), value, 0.00001);
  EXPECT_THAT(casted_node->type(), IsValidType());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_StringValue_GetCorrectStringValue)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  identifiers_context_mock ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto value = cmsl::string_view{ "\"42\"" };
  const auto token = token_string("\"42\"");
  ast::string_value_node node(token);

  EXPECT_CALL(ctx, find_type(_)).WillOnce(Return(&valid_type));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());
  const auto casted_node =
    dynamic_cast<const string_value_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
  EXPECT_THAT(casted_node->value(), Eq("42"));
  EXPECT_THAT(casted_node->type(), IsValidType());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_Identifier_GetCorrectIdentifierNode)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto id_token = token_identifier("foo");
  const ast::id_node node{ id_token };

  EXPECT_CALL(ids_ctx, type_of(id_token.str())).WillOnce(Return(&valid_type));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());
  const auto casted_node =
    dynamic_cast<const id_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
  EXPECT_THAT(casted_node->id(), Eq(id_token));
  EXPECT_THAT(casted_node->type(),
              IsValidType()); // Todo: compare by reference
}

TEST_F(
  SemaBuilderAstVisitorTest,
  Visit_VariableDeclarationWithoutInitialization_GetVariableDeclarationNodeWithoutInitialization)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);
  const auto type_ref = ast::type_representation{ token_identifier() };
  const auto name_token = token_identifier("foo");

  auto variable_node = create_variable_declaration_node(type_ref, name_token);

  EXPECT_CALL(ids_ctx, register_identifier(name_token, Ref(valid_type)));

  EXPECT_CALL(ctx, find_type(_)).WillOnce(Return(&valid_type));

  visitor.visit(*variable_node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<variable_declaration_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->type(), IsValidType());
  EXPECT_THAT(casted_node->name(), Eq(name_token));
  EXPECT_THAT(casted_node->initialization(), IsNull());
}

TEST_F(
  SemaBuilderAstVisitorTest,
  Visit_VariableDeclarationWithInitialization_GetVariableDeclarationNodeWithInitialization)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);
  const auto type_representation =
    ast::type_representation{ token_identifier() };
  const auto name_token = token_identifier("foo");

  const auto initialization_value = int_t{ 42 };
  const auto initialization_token = token_integer("42");
  auto initializaton_node =
    std::make_unique<ast::int_value_node>(initialization_token);

  auto variable_node = create_variable_declaration_node(
    type_representation, name_token,
    ast::variable_declaration_node::initialization_values{
      tmp_token, std::move(initializaton_node) });

  EXPECT_CALL(ctx, find_type(_)).WillRepeatedly(Return(&valid_type));

  EXPECT_CALL(ids_ctx, register_identifier(name_token, Ref(valid_type)));

  visitor.visit(*variable_node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<variable_declaration_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->type(), IsValidType());
  EXPECT_THAT(casted_node->name(), Eq(name_token));
  EXPECT_THAT(casted_node->initialization(), NotNull());

  const auto casted_initialization_node =
    dynamic_cast<const int_value_node*>(casted_node->initialization());
  ASSERT_THAT(casted_initialization_node, NotNull());

  EXPECT_THAT(casted_initialization_node->value(), Eq(initialization_value));
}

TEST_F(SemaBuilderAstVisitorTest, Visit_Return)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  auto expr_node = std::make_unique<ast::int_value_node>(token_integer("42"));
  ast::return_node ret_node(tmp_token, std::move(expr_node), tmp_token);

  EXPECT_CALL(ctx, find_type(_)).WillOnce(Return(&valid_type));

  visitor.visit(ret_node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<return_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  // Todo check for void type
  EXPECT_THAT(casted_node->type(), IsValidType());
}

TEST_F(
  SemaBuilderAstVisitorTest,
  Visit_FunctionCallWithoutParameters_GetFunctionCallNodeWithoutParameters)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<sema_function_mock> function_mock;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto fun_name_token = token_identifier("foo");
  function_signature signature;
  signature.name = fun_name_token;
  auto node = create_function_call(fun_name_token);

  const auto lookup_result = function_lookup_result_t{ { &function_mock } };
  EXPECT_CALL(ctx, find_function(fun_name_token))
    .WillOnce(Return(lookup_result));

  EXPECT_CALL(function_mock, context()).WillRepeatedly(ReturnRef(ctx));

  EXPECT_CALL(ctx, type())
    .WillRepeatedly(Return(sema_context::context_type::namespace_));

  EXPECT_CALL(function_mock, return_type())
    .WillRepeatedly(ReturnRef(valid_type));

  EXPECT_CALL(function_mock, signature()).WillRepeatedly(ReturnRef(signature));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<function_call_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->type(), IsValidType());
  EXPECT_THAT(casted_node->param_expressions().size(), Eq(0u));
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_FunctionCallWithParameters_GetFunctionCallNodeWithParameters)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<sema_function_mock> function_mock;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  // It's crucial that fun name token has different text than valid_type's
  // name. Thanks to that we're not going to call a constructor.
  const auto fun_name_token = token_identifier("not_a_constructor");
  const auto param1_id_token = token_identifier("bar");
  const auto param2_id_token = token_identifier("baz");

  function_signature signature;
  signature.name = fun_name_token;
  signature.params.emplace_back(
    parameter_declaration{ valid_type, param1_id_token });
  signature.params.emplace_back(
    parameter_declaration{ valid_type, param2_id_token });

  const auto lookup_result = function_lookup_result_t{ { &function_mock } };
  EXPECT_CALL(ctx, find_function(fun_name_token))
    .WillOnce(Return(lookup_result));

  EXPECT_CALL(function_mock, context()).WillRepeatedly(ReturnRef(ctx));

  EXPECT_CALL(ctx, type())
    .WillRepeatedly(Return(sema_context::context_type::namespace_));

  EXPECT_CALL(function_mock, signature()).WillRepeatedly(ReturnRef(signature));

  EXPECT_CALL(function_mock, return_type())
    .WillRepeatedly(ReturnRef(valid_type));

  EXPECT_CALL(ids_ctx, type_of(param1_id_token.str()))
    .WillRepeatedly(Return(&valid_type));

  EXPECT_CALL(ids_ctx, type_of(param2_id_token.str()))
    .WillRepeatedly(Return(&valid_type));

  // Todo: use mocks
  auto param1_ast_node = std::make_unique<ast::id_node>(param1_id_token);
  auto param2_ast_node = std::make_unique<ast::id_node>(param2_id_token);

  ast::function_call_node::params_t ast_params;
  ast_params.emplace_back(std::move(param1_ast_node));
  ast_params.emplace_back(std::move(param2_ast_node));
  auto node = create_function_call(fun_name_token, std::move(ast_params));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<function_call_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->type(), IsValidType());
  const auto expected_number_of_params{ 2u };
  EXPECT_THAT(casted_node->param_expressions().size(),
              Eq(expected_number_of_params));

  // Todo: consider checking params one by one
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_FunctionCallWithAddSubdirectoryName_CallsAddSubdirectoryHandler)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<sema_function_mock> function_mock;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto fun_name_token = token_identifier("add_subdirectory");
  const auto name_param_token = token_string("\"subdir\"");

  function_signature signature;
  signature.name = fun_name_token;
  signature.params.emplace_back(
    parameter_declaration{ valid_type, name_param_token });

  auto param1_ast_node =
    std::make_unique<ast::string_value_node>(name_param_token);

  ast::function_call_node::params_t ast_params;
  ast_params.emplace_back(std::move(param1_ast_node));
  auto node = create_function_call(fun_name_token, std::move(ast_params));

  const auto lookup_result = function_lookup_result_t{ { &function_mock } };

  EXPECT_CALL(function_mock, context()).WillRepeatedly(ReturnRef(ctx));

  EXPECT_CALL(function_mock, signature()).WillRepeatedly(ReturnRef(signature));

  EXPECT_CALL(function_mock, return_type())
    .WillRepeatedly(ReturnRef(valid_type));

  EXPECT_CALL(ctx, type())
    .WillRepeatedly(Return(sema_context::context_type::namespace_));

  EXPECT_CALL(ctx, find_type(_)).WillRepeatedly(Return(&valid_type));

  EXPECT_CALL(m_add_subdirectory_mock,
              handle_add_subdirectory(cmsl::string_view{ "subdir" }, _))
    .WillRepeatedly(Return(&function_mock));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<add_subdirectory_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->type(), IsValidType());
  // No parameters, because sema builder removes the directory name parameter.
  EXPECT_THAT(casted_node->param_expressions().size(), Eq(0u));
}

TEST_F(
  SemaBuilderAstVisitorTest,
  Visit_ConstructorCallWithoutParameters_GetConstructorCallNodeWithoutParameters)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<sema_function_mock> function_mock;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  // It's crucial that fun name token has same text as valid_type's name.
  // Thanks to that we're going to call a constructor.
  const auto fun_name_token = token_identifier("foo");
  function_signature signature;
  signature.name = fun_name_token;
  auto node = create_function_call(fun_name_token);

  const auto lookup_result = function_lookup_result_t{ { &function_mock } };
  EXPECT_CALL(ctx, find_function(fun_name_token))
    .WillOnce(Return(lookup_result));

  EXPECT_CALL(function_mock, context()).WillRepeatedly(ReturnRef(ctx));

  EXPECT_CALL(ctx, type())
    .WillRepeatedly(Return(sema_context::context_type::namespace_));

  EXPECT_CALL(function_mock, return_type())
    .WillRepeatedly(ReturnRef(valid_type));

  EXPECT_CALL(function_mock, signature()).WillRepeatedly(ReturnRef(signature));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<function_call_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->type(), IsValidType());
  EXPECT_THAT(casted_node->param_expressions().size(), Eq(0u));
}

TEST_F(
  SemaBuilderAstVisitorTest,
  Visit_MemberFunctionCallWithoutParameters_GetMemberFunctionCallNodeWithoutParameters)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<sema_function_mock> function_mock;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto lhs_id_token = token_identifier("foo");
  const sema_type lhs_type{ ctx,
                            ast::type_representation{ lhs_id_token },
                            {} };
  auto lhs_ast_node = std::make_unique<ast::id_node>(lhs_id_token);

  const auto fun_name_token = token_identifier("bar");
  function_signature signature;
  signature.name = fun_name_token;
  auto node =
    create_member_function_call_node(std::move(lhs_ast_node), fun_name_token);

  EXPECT_CALL(ids_ctx, type_of(lhs_id_token.str()))
    .WillOnce(Return(&lhs_type));

  const auto lookup_result =
    single_scope_function_lookup_result_t{ &function_mock };
  EXPECT_CALL(ctx, find_function_in_this_scope(fun_name_token))
    .WillOnce(Return(lookup_result));

  EXPECT_CALL(function_mock, signature()).WillRepeatedly(ReturnRef(signature));

  EXPECT_CALL(function_mock, return_type())
    .WillRepeatedly(ReturnRef(valid_type));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<member_function_call_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->type(), IsValidType());
  const auto expected_number_of_params{ 0u };
  EXPECT_THAT(casted_node->param_expressions().size(),
              Eq(expected_number_of_params));
}

TEST_F(
  SemaBuilderAstVisitorTest,
  Visit_MemberFunctionCallWithParameters_GetMemberFunctionCallNodeWithParameters)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<sema_function_mock> function_mock;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto lhs_id_token = token_identifier("foo");
  const sema_type lhs_type{ ctx,
                            ast::type_representation{ lhs_id_token },
                            {} };
  auto lhs_ast_node = std::make_unique<ast::id_node>(lhs_id_token);

  const auto param1_id_token = token_identifier("baz");
  const auto param2_id_token = token_identifier("qux");

  // Todo: use mocks
  auto param1_ast_node = std::make_unique<ast::id_node>(param1_id_token);
  auto param2_ast_node = std::make_unique<ast::id_node>(param2_id_token);
  ast::function_call_node::params_t ast_params;
  ast_params.emplace_back(std::move(param1_ast_node));
  ast_params.emplace_back(std::move(param2_ast_node));

  const auto fun_name_token = token_identifier("bar");
  function_signature signature;
  signature.name = fun_name_token;
  signature.params.emplace_back(
    parameter_declaration{ valid_type, param1_id_token });
  signature.params.emplace_back(
    parameter_declaration{ valid_type, param2_id_token });
  auto node = create_member_function_call_node(
    std::move(lhs_ast_node), fun_name_token, std::move(ast_params));

  EXPECT_CALL(ids_ctx, type_of(lhs_id_token.str()))
    .WillRepeatedly(Return(&lhs_type));

  const auto lookup_result =
    single_scope_function_lookup_result_t{ &function_mock };
  EXPECT_CALL(ctx, find_function_in_this_scope(fun_name_token))
    .WillOnce(Return(lookup_result));

  EXPECT_CALL(function_mock, signature()).WillRepeatedly(ReturnRef(signature));

  EXPECT_CALL(ids_ctx, type_of(param1_id_token.str()))
    .WillRepeatedly(Return(&valid_type));

  EXPECT_CALL(ids_ctx, type_of(param2_id_token.str()))
    .WillRepeatedly(Return(&valid_type));

  EXPECT_CALL(function_mock, return_type())
    .WillRepeatedly(ReturnRef(valid_type));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<member_function_call_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->type(), IsValidType());
  const auto expected_number_of_params{ 2u };
  EXPECT_THAT(casted_node->param_expressions().size(),
              Eq(expected_number_of_params));

  // Todo: consider checking params one by one
}

// Todo: consider test with nodes inside block
TEST_F(SemaBuilderAstVisitorTest, Visit_Block_GetBlockNode)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  auto block = create_block_node_ptr();

  EXPECT_CALL(ids_ctx, enter_ctx());
  EXPECT_CALL(ids_ctx, leave_ctx());

  visitor.visit(*block);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<block_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  const auto expected_number_of_nodes{ 0u };
  EXPECT_THAT(casted_node->nodes().size(), Eq(expected_number_of_nodes));
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_FunctionWithoutParameters_GetFunctionNodeWithoutParameters)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  auto return_type_token = token_identifier("foo");
  auto return_type_reference = ast::type_representation{ return_type_token };
  auto name_token = token_identifier("bar");
  auto block = create_block_node_ptr();
  auto node = create_user_function_node(return_type_reference, name_token,
                                        std::move(block));

  EXPECT_CALL(ctx, find_type(return_type_reference))
    .WillOnce(Return(&valid_type));

  EXPECT_CALL(ctx, add_function(_));

  // Two scopes: parameters and block
  EXPECT_CALL(ids_ctx, enter_ctx()).Times(2);
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(2);

  visitor.visit(*node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<function_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  const auto expected_number_of_params{ 0u };
  EXPECT_THAT(casted_node->signature().params.size(),
              Eq(expected_number_of_params));
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_FunctionWithParameters_GetFunctionNodeWithParameters)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  auto return_type_token = token_identifier("foo");
  auto return_type_reference = ast::type_representation{ return_type_token };
  auto name_token = token_identifier("bar");
  auto param_type_token = token_identifier("baz");
  auto param_type_reference = ast::type_representation{ param_type_token };
  auto param_name_token = token_identifier("baz");

  ast::user_function_node::params_t params;
  params.emplace_back(
    ast::param_declaration{ param_type_reference, param_name_token });

  auto block = create_block_node_ptr();
  auto node = create_user_function_node(return_type_reference, name_token,
                                        std::move(block), std::move(params));

  EXPECT_CALL(ctx, find_type(return_type_reference))
    .WillOnce(Return(&valid_type));

  EXPECT_CALL(ctx, find_type(param_type_reference))
    .WillOnce(Return(&valid_type));

  EXPECT_CALL(ctx, add_function(_));

  // Two scopes: parameters and block
  EXPECT_CALL(ids_ctx, enter_ctx()).Times(2);
  EXPECT_CALL(ids_ctx, register_identifier(param_name_token, Ref(valid_type)));
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(2);

  visitor.visit(*node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<function_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  const auto expected_number_of_params{ 1u };
  EXPECT_THAT(casted_node->signature().params.size(),
              Eq(expected_number_of_params));
}

TEST_F(SemaBuilderAstVisitorTest, Visit_ClassEmpty_GetEmptyClassNode)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  auto class_name_token = token_identifier("foo");

  ast::class_node node{ tmp_token, class_name_token, tmp_token,
                        {},        tmp_token,        tmp_token };

  const auto class_type_name_ref =
    ast::type_representation{ class_name_token };
  EXPECT_CALL(ctx, find_type_in_this_scope(class_type_name_ref))
    .WillOnce(Return(nullptr));

  EXPECT_CALL(ctx, add_type(_)).Times(2); // Type and reference

  EXPECT_CALL(ids_ctx, enter_ctx());
  EXPECT_CALL(ids_ctx, leave_ctx());

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<class_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->name().str(), Eq(class_name_token.str()));
  ASSERT_THAT(casted_node, NotNull());

  const auto expected_number_of_members{ 0u };
  EXPECT_THAT(casted_node->members().size(), Eq(expected_number_of_members));
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_ClassWithMembers_GetClassNodeWithMembers)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  auto class_name_token = token_identifier("foo");
  const auto member_name_token = token_identifier("bar");
  const auto member_type_token = token_kw_int();
  const auto member_type_reference =
    ast::type_representation{ member_type_token };

  auto member_declaration =
    create_variable_declaration_node(member_type_reference, member_name_token);
  ast::class_node::nodes_t nodes;
  nodes.emplace_back(std::move(member_declaration));

  ast::class_node node{ tmp_token,        class_name_token, tmp_token,
                        std::move(nodes), tmp_token,        tmp_token };

  const auto class_type_name_ref =
    ast::type_representation{ class_name_token };
  EXPECT_CALL(ctx, find_type_in_this_scope(class_type_name_ref))
    .WillOnce(Return(nullptr));

  const auto member_type_name_ref =
    ast::type_representation{ member_type_token };
  EXPECT_CALL(ctx, find_type(member_type_name_ref))
    .WillOnce(Return(&valid_type));

  EXPECT_CALL(ctx, add_type(_)).Times(2); // Type and reference

  EXPECT_CALL(ids_ctx, enter_ctx());
  EXPECT_CALL(ids_ctx,
              register_identifier(member_name_token, Ref(valid_type)));
  EXPECT_CALL(ids_ctx, leave_ctx());

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<class_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->name().str(), Eq(class_name_token.str()));
  ASSERT_THAT(casted_node, NotNull());

  const auto expected_number_of_members{ 1u };
  EXPECT_THAT(casted_node->members().size(), Eq(expected_number_of_members));
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_ClassWithFunctions_GetClassNodeWithFunctions)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto class_name_token = token_identifier("foo");

  const auto function_return_type_token = token_kw_int();
  const auto function_return_type_reference =
    ast::type_representation{ function_return_type_token };
  const auto function_name_token = token_identifier("bar");
  auto function_body = create_block_node_ptr();
  auto function =
    create_user_function_node(function_return_type_reference,
                              function_name_token, std::move(function_body));
  ast::class_node::nodes_t nodes;
  nodes.emplace_back(std::move(function));
  ast::class_node node{ tmp_token,        class_name_token, tmp_token,
                        std::move(nodes), tmp_token,        tmp_token };

  // Class type lookup
  const auto class_type_name_ref =
    ast::type_representation{ class_name_token };
  EXPECT_CALL(ctx, find_type_in_this_scope(class_type_name_ref))
    .WillOnce(Return(nullptr));

  // Function return type lookup
  EXPECT_CALL(ctx, find_type(function_return_type_reference))
    .WillOnce(Return(&valid_type));

  EXPECT_CALL(ctx, add_type(_)).Times(2); // Type and reference

  // There are three identifier contextes: class, function parameters and
  // function body.
  EXPECT_CALL(ids_ctx, enter_ctx()).Times(3);
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(3);

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<class_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->name().str(), Eq(class_name_token.str()));
  ASSERT_THAT(casted_node, NotNull());

  const auto expected_number_of_members{ 0u };
  EXPECT_THAT(casted_node->members().size(), Eq(expected_number_of_members));
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_ClassWithFunctionsAndMembers_GetClassNodeWithFunctionsAndMembers)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto class_name_token = token_identifier("foo");

  const auto member_name_token = token_identifier("baz");
  const auto member_type_token = token_kw_int();
  const auto member_type_reference =
    ast::type_representation{ member_type_token };
  auto member_declaration =
    create_variable_declaration_node(member_type_reference, member_name_token);

  const auto function_return_type_token = token_kw_int();
  const auto function_return_type_reference =
    ast::type_representation{ function_return_type_token };
  const auto function_name_token = token_identifier("bar");
  auto function_body = create_block_node_ptr();
  auto function =
    create_user_function_node(function_return_type_reference,
                              function_name_token, std::move(function_body));

  ast::class_node::nodes_t nodes;
  nodes.emplace_back(std::move(member_declaration));
  nodes.emplace_back(std::move(function));
  ast::class_node node{ tmp_token,        class_name_token, tmp_token,
                        std::move(nodes), tmp_token,        tmp_token };

  // Class type lookup.
  const auto class_type_name_ref =
    ast::type_representation{ class_name_token };
  EXPECT_CALL(ctx, find_type_in_this_scope(class_type_name_ref))
    .WillOnce(Return(nullptr));

  // Function return type and member type lookup.
  EXPECT_CALL(ctx, find_type(member_type_reference))
    .Times(2)
    .WillOnce(Return(&valid_type))
    .WillOnce(Return(&valid_type));

  EXPECT_CALL(ctx, add_type(_)).Times(2); // Type and reference

  // There are three identifier contextes: class, function parameters and
  // function body. Member is registered in class context.
  EXPECT_CALL(ids_ctx, enter_ctx()).Times(3);
  EXPECT_CALL(ids_ctx,
              register_identifier(member_name_token, Ref(valid_type)));
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(3);

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<class_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->name().str(), Eq(class_name_token.str()));
  ASSERT_THAT(casted_node, NotNull());

  const auto expected_number_of_members{ 1u };
  EXPECT_THAT(casted_node->members().size(), Eq(expected_number_of_members));
}

TEST_F(SemaBuilderAstVisitorTest, Visit_WhileNode)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto condition_identifier_token = token_identifier("foo");
  auto condition_ast_node =
    std::make_unique<ast::id_node>(condition_identifier_token);
  auto body = create_block_node_ptr();
  auto conditional_ast_node = std::make_unique<ast::conditional_node>(
    tmp_token, std::move(condition_ast_node), tmp_token, std::move(body));

  ast::while_node node(tmp_token, std::move(conditional_ast_node));

  EXPECT_CALL(ids_ctx, type_of(condition_identifier_token.str()))
    .WillOnce(Return(&valid_type));

  EXPECT_CALL(ids_ctx, enter_ctx()).Times(2);
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(2);

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<while_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_IfNode_GetIfNode)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto condition_identifier_token = token_identifier("foo");
  auto condition_ast_node =
    std::make_unique<ast::id_node>(condition_identifier_token);
  auto body = create_block_node_ptr();
  auto conditional_ast_node = std::make_unique<ast::conditional_node>(
    tmp_token, std::move(condition_ast_node), tmp_token, std::move(body));

  ast::if_else_node::ifs_t ifs;
  ifs.emplace_back(ast::if_else_node::if_values{
    std::nullopt, tmp_token, std::move(conditional_ast_node) });

  ast::if_else_node node(std::move(ifs), std::nullopt);

  EXPECT_CALL(ids_ctx, type_of(condition_identifier_token.str()))
    .WillOnce(Return(&valid_type));

  EXPECT_CALL(ids_ctx, enter_ctx()).Times(2);
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(2);

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<if_else_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  const auto expected_number_of_ifs{ 1u };
  EXPECT_THAT(casted_node->ifs().size(), Eq(expected_number_of_ifs));

  EXPECT_THAT(casted_node->else_body(), IsNull());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_IfElseNode_GetIfElseNode)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto condition_identifier_token = token_identifier("foo");
  auto condition_ast_node =
    std::make_unique<ast::id_node>(condition_identifier_token);
  auto body = create_block_node_ptr();
  auto conditional_ast_node = std::make_unique<ast::conditional_node>(
    tmp_token, std::move(condition_ast_node), tmp_token, std::move(body));

  ast::if_else_node::ifs_t ifs;
  ifs.emplace_back(ast::if_else_node::if_values{
    std::nullopt, tmp_token, std::move(conditional_ast_node) });

  auto else_body = create_block_node_ptr();
  ast::if_else_node::last_else_value else_value{ tmp_token,
                                                 std::move(else_body) };

  ast::if_else_node node(std::move(ifs), std::move(else_value));

  EXPECT_CALL(ids_ctx, type_of(condition_identifier_token.str()))
    .WillOnce(Return(&valid_type));

  EXPECT_CALL(ids_ctx, enter_ctx()).Times(4);
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(4);

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<if_else_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  const auto expected_number_of_ifs{ 1u };
  EXPECT_THAT(casted_node->ifs().size(), Eq(expected_number_of_ifs));

  ASSERT_THAT(casted_node->else_body(), NotNull());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_IfElseIfNode_GetIfElseIfNode)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto condition_identifier_token = token_identifier("foo");
  auto condition_ast_node =
    std::make_unique<ast::id_node>(condition_identifier_token);
  auto body = create_block_node_ptr();
  auto conditional_ast_node = std::make_unique<ast::conditional_node>(
    tmp_token, std::move(condition_ast_node), tmp_token, std::move(body));

  const auto condition_identifier_token_2 = token_identifier("bar");
  auto condition_ast_node_2 =
    std::make_unique<ast::id_node>(condition_identifier_token_2);
  auto body_2 = create_block_node_ptr();
  auto conditional_ast_node_2 = std::make_unique<ast::conditional_node>(
    tmp_token, std::move(condition_ast_node_2), tmp_token, std::move(body_2));

  ast::if_else_node::ifs_t ifs;
  ifs.emplace_back(ast::if_else_node::if_values{
    std::nullopt, tmp_token, std::move(conditional_ast_node) });
  ifs.emplace_back(ast::if_else_node::if_values{
    tmp_token, tmp_token, std::move(conditional_ast_node_2) });

  ast::if_else_node node(std::move(ifs), std::nullopt);

  EXPECT_CALL(ids_ctx, type_of(condition_identifier_token.str()))
    .WillOnce(Return(&valid_type));

  EXPECT_CALL(ids_ctx, type_of(condition_identifier_token_2.str()))
    .WillOnce(Return(&valid_type));

  EXPECT_CALL(ids_ctx, enter_ctx()).Times(4);
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(4);

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<if_else_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  const auto expected_number_of_ifs{ 2u };
  EXPECT_THAT(casted_node->ifs().size(), Eq(expected_number_of_ifs));

  EXPECT_THAT(casted_node->else_body(), IsNull());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_IfElseIfElseNode_GetIfElseIfElseNode)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto condition_identifier_token = token_identifier("foo");
  auto condition_ast_node =
    std::make_unique<ast::id_node>(condition_identifier_token);
  auto body = create_block_node_ptr();
  auto conditional_ast_node = std::make_unique<ast::conditional_node>(
    tmp_token, std::move(condition_ast_node), tmp_token, std::move(body));

  const auto condition_identifier_token_2 = token_identifier("bar");
  auto condition_ast_node_2 =
    std::make_unique<ast::id_node>(condition_identifier_token_2);
  auto body_2 = create_block_node_ptr();
  auto conditional_ast_node_2 = std::make_unique<ast::conditional_node>(
    tmp_token, std::move(condition_ast_node_2), tmp_token, std::move(body_2));

  ast::if_else_node::ifs_t ifs;
  ifs.emplace_back(ast::if_else_node::if_values{
    std::nullopt, tmp_token, std::move(conditional_ast_node) });
  ifs.emplace_back(ast::if_else_node::if_values{
    tmp_token, tmp_token, std::move(conditional_ast_node_2) });

  auto else_body = create_block_node_ptr();
  ast::if_else_node::last_else_value else_value{ tmp_token,
                                                 std::move(else_body) };

  ast::if_else_node node(std::move(ifs), std::move(else_value));

  EXPECT_CALL(ids_ctx, type_of(condition_identifier_token.str()))
    .WillOnce(Return(&valid_type));

  EXPECT_CALL(ids_ctx, type_of(condition_identifier_token_2.str()))
    .WillOnce(Return(&valid_type));

  EXPECT_CALL(ids_ctx, enter_ctx()).Times(6);
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(6);

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<if_else_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  const auto expected_number_of_ifs{ 2u };
  EXPECT_THAT(casted_node->ifs().size(), Eq(expected_number_of_ifs));

  EXPECT_THAT(casted_node->else_body(), NotNull());
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_ClassMemberAccess_GetClassMemberAccessNode)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto lhs_id_token = token_identifier("foo");
  auto lhs_node = std::make_unique<ast::id_node>(lhs_id_token);

  const auto member_name_token = token_identifier("bar");
  std::vector<member_info> members;
  members.emplace_back(member_info{ member_name_token, valid_type });
  const auto lhs_type_name_token = token_identifier("baz");
  sema_type lhs_type{ ctx, ast::type_representation{ lhs_type_name_token },
                      std::move(members) };

  ast::class_member_access_node node{ std::move(lhs_node), tmp_token,
                                      member_name_token };

  EXPECT_CALL(ids_ctx, type_of(lhs_id_token.str()))
    .WillOnce(Return(&lhs_type));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<class_member_access_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->type(), IsValidType());
  EXPECT_THAT(casted_node->member_name(), Eq(member_name_token));
}

TEST_F(SemaBuilderAstVisitorTest, Visit_TranslationUnit_GetTranslationUnitNode)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const auto variable_name_token = token_identifier("foo");
  const auto variable_type_token = token_kw_int();
  const auto variable_type_reference =
    ast::type_representation{ variable_type_token };
  auto variable_declaration_ast_node = create_variable_declaration_node(
    variable_type_reference, variable_name_token);

  const auto function_return_type_token = token_kw_double();
  const auto function_return_type_reference =
    ast::type_representation{ function_return_type_token };
  const auto function_name_token = token_identifier("bar");
  auto function_body = create_block_node_ptr();
  auto function_ast_node =
    create_user_function_node(function_return_type_reference,
                              function_name_token, std::move(function_body));

  const auto class_name_token = token_identifier("baz");
  auto class_ast_node = std::make_unique<ast::class_node>(
    tmp_token, class_name_token, tmp_token, ast::class_node::nodes_t{},
    tmp_token, tmp_token);

  ast::translation_unit_node::nodes_t nodes;
  nodes.emplace_back(std::move(variable_declaration_ast_node));
  nodes.emplace_back(std::move(function_ast_node));
  nodes.emplace_back(std::move(class_ast_node));

  ast::translation_unit_node node{ std::move(nodes) };

  EXPECT_CALL(ctx, find_type(variable_type_reference))
    .WillOnce(Return(&valid_type));
  EXPECT_CALL(ctx, find_type(function_return_type_reference))
    .WillOnce(Return(&valid_type));
  const auto class_type_name_ref =
    ast::type_representation{ class_name_token };
  EXPECT_CALL(ctx, find_type_in_this_scope(class_type_name_ref))
    .WillOnce(Return(nullptr));

  EXPECT_CALL(ctx, add_function(_));
  EXPECT_CALL(ctx, add_type(_)).Times(2); // Type and reference

  EXPECT_CALL(ids_ctx,
              register_identifier(variable_name_token, Ref(valid_type)));

  EXPECT_CALL(ids_ctx, enter_ctx()).Times(4);
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(4);

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<translation_unit_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  const auto& translation_unit_nodes = casted_node->nodes();
  const auto expected_number_of_nodes{ 3u };
  ASSERT_THAT(translation_unit_nodes.size(), Eq(expected_number_of_nodes));

  const auto casted_variable_node =
    dynamic_cast<variable_declaration_node*>(translation_unit_nodes[0].get());
  EXPECT_THAT(casted_variable_node, NotNull());

  const auto casted_function_node =
    dynamic_cast<function_node*>(translation_unit_nodes[1].get());
  EXPECT_THAT(casted_function_node, NotNull());

  const auto casted_class_node =
    dynamic_cast<class_node*>(translation_unit_nodes[2].get());
  EXPECT_THAT(casted_class_node, NotNull());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_BinaryOperatorNode)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<sema_function_mock> operator_function;
  auto visitor = create_visitor(errs, ctx, ids_ctx);

  const sema_type lhs_and_rhs_type{ ctx,
                                    ast::type_representation{ token_kw_int() },
                                    {} };
  const auto lhs_id_token = token_identifier("foo");
  auto lhs_node = std::make_unique<ast::id_node>(lhs_id_token);
  const auto rhs_id_token = token_identifier("bar");
  auto rhs_node = std::make_unique<ast::id_node>(rhs_id_token);

  const auto operator_token = token_plus();

  ast::binary_operator_node node{ std::move(lhs_node), operator_token,
                                  std::move(rhs_node) };

  EXPECT_CALL(ids_ctx, type_of(lhs_id_token.str()))
    .WillOnce(Return(&lhs_and_rhs_type));

  EXPECT_CALL(ids_ctx, type_of(rhs_id_token.str()))
    .WillOnce(Return(&lhs_and_rhs_type));

  const auto expected_signature = function_signature{
    operator_token, { parameter_declaration{ lhs_and_rhs_type, lhs_id_token } }
  };
  EXPECT_CALL(operator_function, signature())
    .WillOnce(ReturnRef(expected_signature));

  EXPECT_CALL(operator_function, return_type())
    .WillOnce(ReturnRef(lhs_and_rhs_type));

  // Find operator member function.
  const auto lookup_result =
    single_scope_function_lookup_result_t{ &operator_function };
  EXPECT_CALL(ctx, find_function_in_this_scope(operator_token))
    .WillOnce(Return(lookup_result));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<binary_operator_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
  EXPECT_THAT(&casted_node->lhs().type(), Eq(&lhs_and_rhs_type));
  EXPECT_THAT(&casted_node->rhs().type(), Eq(&lhs_and_rhs_type));
}
}