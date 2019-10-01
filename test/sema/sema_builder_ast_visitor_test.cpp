#include "sema/sema_builder_ast_visitor.hpp"

#include "ast/block_node.hpp"
#include "ast/break_node.hpp"
#include "ast/class_node.hpp"
#include "ast/conditional_node.hpp"
#include "ast/enum_node.hpp"
#include "ast/for_node.hpp"
#include "ast/if_else_node.hpp"
#include "ast/import_node.hpp"
#include "ast/infix_nodes.hpp"
#include "ast/return_node.hpp"
#include "ast/ternary_operator_node.hpp"
#include "ast/translation_unit_node.hpp"
#include "ast/user_function_node.hpp"
#include "ast/variable_declaration_node.hpp"
#include "ast/while_node.hpp"
#include "errors/errors_observer.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/factories.hpp"
#include "sema/factories_provider.hpp"
#include "sema/types_context.hpp"
#include "test/common/tokens.hpp"
#include "test/errors_observer_mock/errors_observer_mock.hpp"
#include "test/sema/mock/add_subdirectory_semantic_handler_mock.hpp"
#include "test/sema/mock/enum_values_context_mock.hpp"
#include "test/sema/mock/expression_node_mock.hpp"
#include "test/sema/mock/functions_context_mock.hpp"
#include "test/sema/mock/identifiers_context_mock.hpp"
#include "test/sema/mock/import_handler_mock.hpp"
#include "test/sema/mock/sema_context_mock.hpp"
#include "test/sema/mock/sema_function_mock.hpp"
#include "test/sema/mock/types_context_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl::ast {
inline void PrintTo(const qualified_name_t& names, ::std::ostream* out)
{
  *out << '"';
  for (const auto& n : names) {
    *out << n.name.str();
    if (n.coloncolon) {
      *out << "::";
    }
  }
  *out << '"';
}
}

namespace cmsl::sema::test {
using ::testing::StrictMock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::ByMove;
using ::testing::Ref;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::AnyNumber;
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

const sema_context_impl builtin_context_fake{ "" };

struct type_data
{
  ast::type_representation representation;
  sema_context_impl ctx;
  sema_type ty{ ctx, representation, {} };
  std::vector<ast::name_with_coloncolon> qualified_names =
    representation.qual_name().names();
};

type_data create_type_data(lexer::token token)
{
  return type_data{ ast::type_representation{ ast::qualified_name{ token } },
                    sema_context_impl{ std::string{ token.str() },
                                       &builtin_context_fake } };
}

type_data valid_type_data = create_type_data(token_identifier("foo"));
const sema_type valid_type_reference{ sema_type_reference{
  valid_type_data.ty } };
type_data different_type_data =
  create_type_data(token_identifier("different_type"));

const auto tmp_token = token_identifier("");

class SemaBuilderAstVisitorTest : public ::testing::Test
{
protected:
  factories_provider m_factories;
  add_subdirectory_semantic_handler_mock m_add_subdirectory_mock;
  import_handler_mock m_import_handler_mock;
  builtin_token_provider m_token_provider{ "" };
  parsing_context m_parsing_ctx;

  type_data m_void_type_data = create_type_data(token_kw_void());
  type_data m_bool_type_data = create_type_data(token_kw_bool());
  type_data m_int_type_data = create_type_data(token_kw_int());
  type_data m_double_type_data = create_type_data(token_kw_double());
  type_data m_string_type_data = create_type_data(token_kw_string());
  type_data m_version_type_data = create_type_data(token_kw_version());
  type_data m_library_type_data = create_type_data(token_kw_library());
  type_data m_executable_type_data = create_type_data(token_kw_executable());
  type_data m_project_type_data = create_type_data(token_kw_project());

  auto create_types_factory_and_visitor(errs_t& errs, sema_context& ctx,
                                        enum_values_context& enums_ctx,
                                        identifiers_context& ids_ctx,
                                        types_context& types_ctx,
                                        functions_context& functions_ctx,
                                        parsing_context& parsing_ctx)
  {
    const auto builtin_types =
      builtin_types_accessor{ .void_ = m_void_type_data.ty,
                              .bool_ = m_bool_type_data.ty,
                              .bool_ref = valid_type_data.ty,
                              .int_ = m_int_type_data.ty,
                              .int_ref = valid_type_data.ty,
                              .double_ = m_double_type_data.ty,
                              .double_ref = valid_type_data.ty,
                              .string = m_string_type_data.ty,
                              .string_ref = valid_type_data.ty };

    auto qualified_ctxs =
      qualified_contextes_refs{ enums_ctx, functions_ctx, ids_ctx, types_ctx };

    auto tmp_members =
      sema_builder_ast_visitor_members{ ctx,
                                        ctx,
                                        errs.observer,
                                        qualified_ctxs,
                                        m_factories,
                                        m_token_provider,
                                        parsing_ctx,
                                        builtin_types,
                                        m_add_subdirectory_mock,
                                        m_import_handler_mock };

    auto members = std::make_unique<sema_builder_ast_visitor_members>(
      std::move(tmp_members));

    auto visitor = sema_builder_ast_visitor{ *members };

    return std::make_pair(std::move(members), std::move(visitor));
  }

  auto create_types_factory_and_visitor(errs_t& errs, sema_context& ctx,
                                        enum_values_context& enums_ctx,
                                        identifiers_context& ids_ctx,
                                        types_context& types_ctx,
                                        functions_context& functions_ctx)
  {
    return create_types_factory_and_visitor(
      errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx, m_parsing_ctx);
  }

  ast::function_call_node create_function_call(
    token_t name, ast::function_call_node::params_t params = {})
  {
    std::vector<ast::name_with_coloncolon> names{ { name } };
    return ast::function_call_node{ names, tmp_token, std::move(params),
                                    tmp_token };
  }

  std::unique_ptr<ast::function_call_node> create_function_call_ptr(
    token_t name, ast::function_call_node::params_t params = {})
  {
    std::vector<ast::name_with_coloncolon> names{ { name } };
    return std::make_unique<ast::function_call_node>(
      names, tmp_token, std::move(params), tmp_token);
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

  std::unique_ptr<ast::standalone_variable_declaration_node>
  create_standalone_variable_declaration_node(
    ast::type_representation type, token_t name,
    std::optional<
      ast::standalone_variable_declaration_node::initialization_values_t>
      initialization = std::nullopt,
    std::optional<lexer::token> export_kw = std::nullopt)
  {
    auto var_decl = std::make_unique<ast::variable_declaration_node>(
      export_kw, std::move(type), name, std::move(initialization));
    return std::make_unique<ast::standalone_variable_declaration_node>(
      std::move(var_decl), tmp_token);
  }

  std::unique_ptr<ast::user_function_node> create_user_function_node(
    ast::type_representation type, token_t name,
    std::unique_ptr<ast::block_node> body,
    ast::user_function_node::params_t params = {},
    std::optional<token_t> export_kw = std::nullopt)
  {
    return std::make_unique<ast::user_function_node>(
      export_kw, std::move(type), name, tmp_token, std::move(params),
      tmp_token, std::move(body));
  }

  std::vector<ast::name_with_coloncolon> create_qualified_name(token_t name)
  {
    return { { name } };
  }

  std::unique_ptr<ast::id_node> create_ast_id_node(token_t name)
  {
    return std::make_unique<ast::id_node>(create_qualified_name(name));
  }
};

MATCHER(IsValidType, "")
{
  return arg == valid_type_data.ty;
}

MATCHER(IsValidTypeReference, "")
{
  return arg == valid_type_reference;
}

TEST_F(SemaBuilderAstVisitorTest, Visit_BoolValue_GetCorrectBoolValue)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  identifiers_context_mock ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto value = true;
  const auto token = token_kw_true();
  ast::bool_value_node node(token);

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());
  const auto casted_node =
    dynamic_cast<const bool_value_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
  EXPECT_THAT(casted_node->value(), Eq(value));
  EXPECT_THAT(casted_node->type(), Eq(std::cref(m_bool_type_data.ty)));
}

TEST_F(SemaBuilderAstVisitorTest, Visit_IntValue_GetCorrectIntValue)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  identifiers_context_mock ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto value = int_t{ 42 };
  const auto token = token_integer("42");
  ast::int_value_node node(token);

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());
  const auto casted_node =
    dynamic_cast<const int_value_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
  EXPECT_THAT(casted_node->value(), Eq(value));
  EXPECT_EQ(casted_node->type(), m_int_type_data.ty);
}

TEST_F(SemaBuilderAstVisitorTest, Visit_DoubleValue_GetCorrectDoubleValue)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  identifiers_context_mock ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto value = double{ 4.2 };
  const auto token = token_double("4.2");
  ast::double_value_node node(token);

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());
  const auto casted_node =
    dynamic_cast<const double_value_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
  EXPECT_NEAR(casted_node->value(), value, 0.00001);
  EXPECT_EQ(casted_node->type(), m_double_type_data.ty);
}

TEST_F(SemaBuilderAstVisitorTest, Visit_StringValue_GetCorrectStringValue)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  identifiers_context_mock ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto token = token_string("\"42\"");
  ast::string_value_node node(token);

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());
  const auto casted_node =
    dynamic_cast<const string_value_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
  EXPECT_THAT(casted_node->value(), Eq("42"));
  EXPECT_EQ(casted_node->type(), m_string_type_data.ty);
}

TEST_F(SemaBuilderAstVisitorTest, Visit_Identifier_GetCorrectIdentifierNode)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto id_token = token_identifier("foo");
  const auto q_name = create_qualified_name(id_token);
  const auto identifier_index = 0u;
  const ast::id_node node{ q_name };

  auto expected_info = identifier_info{ valid_type_data.ty, identifier_index };
  EXPECT_CALL(ids_ctx, info_of(_)).WillOnce(Return(expected_info));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());
  const auto casted_node =
    dynamic_cast<const id_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
  EXPECT_THAT(casted_node->names(), Eq(q_name));
  EXPECT_THAT(casted_node->type(),
              IsValidType()); // Todo: compare by reference
}

TEST_F(
  SemaBuilderAstVisitorTest,
  Visit_VariableDeclarationWithoutInitialization_GetVariableDeclarationNodeWithoutInitialization)
{
  errs_t errs;
  sema_context_mock ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);
  const auto name_token = token_identifier("foo");

  auto variable_node = create_standalone_variable_declaration_node(
    m_int_type_data.representation, name_token);

  EXPECT_CALL(ids_ctx, register_identifier(_, _, _));

  const auto expected_found_void =
    types_context::type_with_reference{ m_void_type_data.ty,
                                        m_void_type_data.ty };
  const auto expected_found_int =
    types_context::type_with_reference{ m_int_type_data.ty,
                                        m_int_type_data.ty };
  EXPECT_CALL(types_ctx, find(m_void_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_void));
  EXPECT_CALL(types_ctx, find(m_int_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_int));

  visitor.visit(*variable_node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<variable_declaration_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(&casted_node->type(), &m_int_type_data.ty);
  EXPECT_THAT(casted_node->name(), Eq(name_token));
  EXPECT_THAT(casted_node->initialization(), IsNull());
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_VariableDeclarationWithAutoWithoutInitialization_ReportError)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);
  const auto type_ref = ast::type_representation{ token_kw_auto() };
  const auto name_token = token_identifier("foo");

  auto variable_node =
    create_standalone_variable_declaration_node(type_ref, name_token);

  EXPECT_CALL(errs.mock, notify_error(_)).Times(AnyNumber());

  visitor.visit(*variable_node);

  ASSERT_THAT(visitor.m_result_node, IsNull());
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_VariableDeclarationWithVoidType_ReportError)
{
  errs_t errs;
  sema_context_mock ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);
  const auto type_ref = ast::type_representation{ token_kw_void() };
  const auto name_token = token_identifier("foo");

  auto variable_node =
    create_standalone_variable_declaration_node(type_ref, name_token);

  const auto expected_found_void =
    types_context::type_with_reference{ m_void_type_data.ty,
                                        m_void_type_data.ty };
  EXPECT_CALL(types_ctx, find(type_ref.qual_name().names()))
    .WillRepeatedly(Return(expected_found_void));

  EXPECT_CALL(errs.mock, notify_error(_)).Times(AnyNumber());

  visitor.visit(*variable_node);

  ASSERT_THAT(visitor.m_result_node, IsNull());
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_ExportedVariableDeclarationInLocalContext_ReportError)
{
  errs_t errs;
  sema_context_mock ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);
  const auto type_ref = ast::type_representation{ token_kw_void() };
  const auto name_token = token_identifier("foo");

  auto variable_node = create_standalone_variable_declaration_node(
    type_ref, name_token, std::nullopt, token_kw_export());

  EXPECT_CALL(ids_ctx, is_in_global_ctx()).WillRepeatedly(Return(false));

  EXPECT_CALL(errs.mock, notify_error(_)).Times(AnyNumber());

  visitor.visit(*variable_node);

  ASSERT_THAT(visitor.m_result_node, IsNull());
}

TEST_F(
  SemaBuilderAstVisitorTest,
  Visit_VariableDeclarationWithInitialization_GetVariableDeclarationNodeWithInitialization)
{
  errs_t errs;
  sema_context_mock ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto name_token = token_identifier("foo");

  const auto initialization_value = int_t{ 42 };
  const auto initialization_token = token_integer("42");
  auto initializaton_node =
    std::make_unique<ast::int_value_node>(initialization_token);

  auto variable_node = create_standalone_variable_declaration_node(
    m_int_type_data.representation, name_token,
    ast::standalone_variable_declaration_node::initialization_values_t{
      tmp_token, std::move(initializaton_node) });

  const auto expected_found_int =
    types_context::type_with_reference{ m_int_type_data.ty,
                                        m_int_type_data.ty };
  EXPECT_CALL(types_ctx, find(m_int_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_int));

  EXPECT_CALL(ids_ctx, register_identifier(_, _, _));

  visitor.visit(*variable_node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<variable_declaration_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(&casted_node->type(), &m_int_type_data.ty);
  EXPECT_THAT(casted_node->name(), Eq(name_token));
  EXPECT_THAT(casted_node->initialization(), NotNull());

  const auto casted_initialization_node =
    dynamic_cast<const int_value_node*>(casted_node->initialization());
  ASSERT_THAT(casted_initialization_node, NotNull());

  EXPECT_THAT(casted_initialization_node->value(), Eq(initialization_value));
}

TEST_F(
  SemaBuilderAstVisitorTest,
  Visit_VariableDeclarationWithAutoAndInitialization_GetVariableDeclarationNodeWithInitializationAndTypeDeducedFromInitialization)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);
  const auto type_representation =
    ast::type_representation{ token_kw_auto(),
                              ast::type_representation::is_reference_tag{} };
  const auto name_token = token_identifier("foo");

  const auto initialization_token = token_identifier("bar");
  auto initializaton_node = std::make_unique<ast::id_node>(
    create_qualified_name(initialization_token));

  auto variable_node = create_standalone_variable_declaration_node(
    type_representation, name_token,
    ast::standalone_variable_declaration_node::initialization_values_t{
      tmp_token, std::move(initializaton_node) });

  EXPECT_CALL(ctx, find_type(_)).WillRepeatedly(Return(&valid_type_data.ty));
  EXPECT_CALL(ctx, find_reference_for(Ref(valid_type_data.ty)))
    .WillRepeatedly(Return(&valid_type_reference));

  auto expected_info = identifier_info{ valid_type_data.ty, 0u };
  EXPECT_CALL(ids_ctx, info_of(_)).WillRepeatedly(Return(expected_info));
  EXPECT_CALL(ids_ctx, register_identifier(_, _, _));

  visitor.visit(*variable_node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<variable_declaration_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->type(), IsValidTypeReference());
  EXPECT_THAT(casted_node->name(), Eq(name_token));
  EXPECT_THAT(casted_node->initialization(), NotNull());

  const auto casted_initialization_node =
    dynamic_cast<const cast_to_reference_node*>(casted_node->initialization());
  ASSERT_THAT(casted_initialization_node, NotNull());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_Return)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<sema_function_mock> function;
  StrictMock<identifiers_context_mock> ids_ctx;
  function_parsing_context function_parsing_ctx{ nullptr, &function, {} };
  parsing_context parsing_ctx{ function_parsing_ctx };
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx, parsing_ctx);

  auto expr_node = std::make_unique<ast::int_value_node>(token_integer("42"));
  ast::return_node ret_node(tmp_token, std::move(expr_node), tmp_token);

  EXPECT_CALL(function, return_type())
    .WillRepeatedly(ReturnRef(m_int_type_data.ty));
  EXPECT_CALL(function, try_return_type())
    .WillRepeatedly(Return(&m_int_type_data.ty));

  function_signature signature{ token_identifier("foo") };
  EXPECT_CALL(function, signature()).WillRepeatedly(ReturnRef(signature));

  visitor.visit(ret_node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<return_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  // Todo check for void type
  EXPECT_EQ(casted_node->type(), m_int_type_data.ty);
}

TEST_F(SemaBuilderAstVisitorTest, Visit_ReturnWithMismatchedType_ReportsError)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<sema_function_mock> function;
  StrictMock<identifiers_context_mock> ids_ctx;
  function_parsing_context function_parsing_ctx{ nullptr, &function, {} };
  parsing_context parsing_ctx{ function_parsing_ctx };
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx, parsing_ctx);

  auto expr_node = std::make_unique<ast::int_value_node>(token_integer("42"));
  ast::return_node ret_node(tmp_token, std::move(expr_node), tmp_token);

  EXPECT_CALL(function, return_type())
    .WillRepeatedly(ReturnRef(m_void_type_data.ty));
  EXPECT_CALL(function, try_return_type())
    .WillRepeatedly(Return(&m_void_type_data.ty));
  EXPECT_CALL(errs.mock, notify_error(_)).Times(AnyNumber());

  const auto function_name_token = token_identifier("foo");
  function_signature signature{ function_name_token };
  EXPECT_CALL(function, signature()).WillRepeatedly(ReturnRef(signature));

  visitor.visit(ret_node);

  ASSERT_THAT(visitor.m_result_node, IsNull());
}

TEST_F(
  SemaBuilderAstVisitorTest,
  Visit_FunctionCallWithoutParameters_GetFunctionCallNodeWithoutParameters)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<sema_function_mock> function_mock;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto fun_name_token = token_identifier("foo");
  const auto qualified_fun_name = create_qualified_name(fun_name_token);
  function_signature signature;
  signature.name = fun_name_token;
  auto node = create_function_call(fun_name_token);

  const auto lookup_result = function_lookup_result_t{ { &function_mock } };
  EXPECT_CALL(functions_ctx, find(qualified_fun_name))
    .WillOnce(Return(lookup_result));

  EXPECT_CALL(function_mock, context()).WillRepeatedly(ReturnRef(ctx));

  EXPECT_CALL(ctx, type())
    .WillRepeatedly(Return(sema_context::context_type::namespace_));

  EXPECT_CALL(function_mock, return_type())
    .WillRepeatedly(ReturnRef(valid_type_data.ty));
  EXPECT_CALL(function_mock, try_return_type())
    .WillRepeatedly(Return(&valid_type_data.ty));

  EXPECT_CALL(types_ctx, find(qualified_fun_name))
    .WillRepeatedly(Return(std::nullopt));

  EXPECT_CALL(function_mock, signature()).WillRepeatedly(ReturnRef(signature));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<function_call_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  EXPECT_THAT(casted_node->type(), IsValidType());
  EXPECT_THAT(casted_node->param_expressions().size(), Eq(0u));
}

TEST_F(SemaBuilderAstVisitorTest, Visit_NotExistingFunctionCall_ReportsError)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto fun_name_token = token_identifier("foo");
  const auto qualified_fun_name = create_qualified_name(fun_name_token);
  function_signature signature;
  signature.name = fun_name_token;
  auto node = create_function_call(fun_name_token);

  const auto lookup_result = function_lookup_result_t{};
  EXPECT_CALL(functions_ctx, find(qualified_fun_name))
    .WillOnce(Return(lookup_result));

  EXPECT_CALL(types_ctx, find(qualified_fun_name))
    .WillRepeatedly(Return(std::nullopt));

  EXPECT_CALL(errs.mock, notify_error(_)).Times(AnyNumber());

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, IsNull());
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_FunctionCallOfReturnTypeNotKnownYet_RaiseError)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<sema_function_mock> function_mock;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto fun_name_token = token_identifier("foo");
  const auto qualified_fun_name = create_qualified_name(fun_name_token);
  function_signature signature;
  signature.name = fun_name_token;
  auto node = create_function_call(fun_name_token);

  const auto lookup_result = function_lookup_result_t{ { &function_mock } };
  EXPECT_CALL(functions_ctx, find(qualified_fun_name))
    .WillOnce(Return(lookup_result));

  EXPECT_CALL(function_mock, context()).WillRepeatedly(ReturnRef(ctx));

  EXPECT_CALL(ctx, type())
    .WillRepeatedly(Return(sema_context::context_type::namespace_));

  EXPECT_CALL(function_mock, try_return_type())
    .WillRepeatedly(Return(nullptr));

  EXPECT_CALL(types_ctx, find(qualified_fun_name))
    .WillRepeatedly(Return(std::nullopt));

  EXPECT_CALL(function_mock, signature()).WillRepeatedly(ReturnRef(signature));

  EXPECT_CALL(errs.mock, notify_error(_)).Times(AnyNumber());

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, IsNull());
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_FunctionCallWithParameters_GetFunctionCallNodeWithParameters)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<sema_function_mock> function_mock;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  // It's crucial that fun name token has different text than
  // valid_type_data.ty's name. Thanks to that we're not going to call a
  // constructor.
  const auto fun_name_token = token_identifier("not_a_constructor");
  const auto qualified_fun_name = create_qualified_name(fun_name_token);
  const auto param1_id_token = token_identifier("bar");
  const auto param2_id_token = token_identifier("baz");

  function_signature signature;
  signature.name = fun_name_token;
  signature.params.emplace_back(
    parameter_declaration{ valid_type_data.ty, param1_id_token });
  signature.params.emplace_back(
    parameter_declaration{ valid_type_data.ty, param2_id_token });

  const auto lookup_result = function_lookup_result_t{ { &function_mock } };
  EXPECT_CALL(functions_ctx, find(qualified_fun_name))
    .WillOnce(Return(lookup_result));

  EXPECT_CALL(types_ctx, find(qualified_fun_name))
    .WillRepeatedly(Return(std::nullopt));

  EXPECT_CALL(function_mock, context()).WillRepeatedly(ReturnRef(ctx));

  EXPECT_CALL(ctx, type())
    .WillRepeatedly(Return(sema_context::context_type::namespace_));

  EXPECT_CALL(function_mock, signature()).WillRepeatedly(ReturnRef(signature));

  EXPECT_CALL(function_mock, return_type())
    .WillRepeatedly(ReturnRef(valid_type_data.ty));
  EXPECT_CALL(function_mock, try_return_type())
    .WillRepeatedly(Return(&valid_type_data.ty));

  auto expected_info = identifier_info{ valid_type_data.ty, 0u };
  EXPECT_CALL(ids_ctx, info_of(_)).WillRepeatedly(Return(expected_info));

  // Todo: use mocks
  auto param1_ast_node = create_ast_id_node(param1_id_token);
  auto param2_ast_node = create_ast_id_node(param2_id_token);

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
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto fun_name_token = token_identifier("add_subdirectory");
  const auto name_param_token = token_string("\"subdir\"");

  function_signature signature;
  signature.name = fun_name_token;
  signature.params.emplace_back(
    parameter_declaration{ valid_type_data.ty, name_param_token });

  auto param1_ast_node =
    std::make_unique<ast::string_value_node>(name_param_token);

  ast::function_call_node::params_t ast_params;
  ast_params.emplace_back(std::move(param1_ast_node));
  auto node = create_function_call(fun_name_token, std::move(ast_params));

  const auto lookup_result = function_lookup_result_t{ { &function_mock } };

  EXPECT_CALL(function_mock, context()).WillRepeatedly(ReturnRef(ctx));

  EXPECT_CALL(function_mock, signature()).WillRepeatedly(ReturnRef(signature));

  EXPECT_CALL(function_mock, return_type())
    .WillRepeatedly(ReturnRef(valid_type_data.ty));
  EXPECT_CALL(function_mock, try_return_type())
    .WillRepeatedly(Return(&valid_type_data.ty));

  EXPECT_CALL(ctx, type())
    .WillRepeatedly(Return(sema_context::context_type::namespace_));

  EXPECT_CALL(ctx, find_type(_)).WillRepeatedly(Return(&valid_type_data.ty));

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
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  // It's crucial that fun name token has same text as valid_type_data.ty's
  // name. Thanks to that we're going to call a constructor.
  const auto fun_name_token = token_identifier("foo");
  const auto qualified_fun_name = create_qualified_name(fun_name_token);
  function_signature signature;
  signature.name = fun_name_token;
  auto node = create_function_call(fun_name_token);

  const auto lookup_result = function_lookup_result_t{ { &function_mock } };
  EXPECT_CALL(functions_ctx, find(qualified_fun_name))
    .WillOnce(Return(lookup_result));

  const auto expected_found_type =
    types_context::type_with_reference{ valid_type_data.ty,
                                        valid_type_data.ty };
  EXPECT_CALL(types_ctx, find(qualified_fun_name))
    .WillRepeatedly(Return(expected_found_type));

  EXPECT_CALL(function_mock, context()).WillRepeatedly(ReturnRef(ctx));

  EXPECT_CALL(ctx, type())
    .WillRepeatedly(Return(sema_context::context_type::namespace_));

  EXPECT_CALL(function_mock, return_type())
    .WillRepeatedly(ReturnRef(valid_type_data.ty));
  EXPECT_CALL(function_mock, try_return_type())
    .WillRepeatedly(Return(&valid_type_data.ty));

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
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto lhs_id_token = token_identifier("foo");
  const sema_type lhs_type{ ctx,
                            ast::type_representation{ lhs_id_token },
                            {} };
  auto lhs_ast_node = create_ast_id_node(lhs_id_token);

  const auto fun_name_token = token_identifier("bar");
  function_signature signature;
  signature.name = fun_name_token;
  auto node =
    create_member_function_call_node(std::move(lhs_ast_node), fun_name_token);

  auto expected_info = identifier_info{ lhs_type, 0u };
  EXPECT_CALL(ids_ctx, info_of(_)).WillOnce(Return(expected_info));

  const auto lookup_result =
    single_scope_function_lookup_result_t{ &function_mock };
  EXPECT_CALL(ctx, find_function_in_this_scope(fun_name_token))
    .WillOnce(Return(lookup_result));

  EXPECT_CALL(function_mock, signature()).WillRepeatedly(ReturnRef(signature));

  EXPECT_CALL(function_mock, return_type())
    .WillRepeatedly(ReturnRef(valid_type_data.ty));
  EXPECT_CALL(function_mock, try_return_type())
    .WillRepeatedly(Return(&valid_type_data.ty));

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
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto lhs_id_token = token_identifier("foo");
  const sema_type lhs_type{ ctx,
                            ast::type_representation{ lhs_id_token },
                            {} };
  auto lhs_ast_node = create_ast_id_node(lhs_id_token);

  const auto param1_id_token = token_identifier("baz");
  const auto param2_id_token = token_identifier("qux");

  // Todo: use mocks
  auto param1_ast_node = create_ast_id_node(param1_id_token);
  auto param2_ast_node = create_ast_id_node(param2_id_token);
  ast::function_call_node::params_t ast_params;
  ast_params.emplace_back(std::move(param1_ast_node));
  ast_params.emplace_back(std::move(param2_ast_node));

  const auto fun_name_token = token_identifier("bar");
  function_signature signature;
  signature.name = fun_name_token;
  signature.params.emplace_back(
    parameter_declaration{ valid_type_data.ty, param1_id_token });
  signature.params.emplace_back(
    parameter_declaration{ valid_type_data.ty, param2_id_token });
  auto node = create_member_function_call_node(
    std::move(lhs_ast_node), fun_name_token, std::move(ast_params));

  auto expected_lhs_info = identifier_info{ lhs_type, 0u };
  auto expected_info = identifier_info{ valid_type_data.ty, 0u };
  EXPECT_CALL(ids_ctx, info_of(_))
    .WillOnce(Return(expected_lhs_info))
    .WillOnce(Return(expected_info))
    .WillOnce(Return(expected_info));

  const auto lookup_result =
    single_scope_function_lookup_result_t{ &function_mock };
  EXPECT_CALL(ctx, find_function_in_this_scope(fun_name_token))
    .WillOnce(Return(lookup_result));

  EXPECT_CALL(function_mock, signature()).WillRepeatedly(ReturnRef(signature));

  EXPECT_CALL(function_mock, return_type())
    .WillRepeatedly(ReturnRef(valid_type_data.ty));

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
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  auto block = create_block_node_ptr();

  EXPECT_CALL(ids_ctx, enter_local_ctx());
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
  sema_context_mock ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  auto name_token = token_identifier("bar");
  auto block = create_block_node_ptr();
  auto node = create_user_function_node(m_int_type_data.representation,
                                        name_token, std::move(block));

  const auto expected_found_void =
    types_context::type_with_reference{ m_void_type_data.ty,
                                        m_void_type_data.ty };
  const auto expected_found_int =
    types_context::type_with_reference{ m_int_type_data.ty,
                                        m_int_type_data.ty };
  EXPECT_CALL(types_ctx, find(m_int_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_int));
  EXPECT_CALL(types_ctx, find(m_void_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_void));

  EXPECT_CALL(functions_ctx, find_in_current_scope(_))
    .WillRepeatedly(Return(nullptr));

  EXPECT_CALL(functions_ctx, register_function(name_token, _, _));

  EXPECT_CALL(ctx, add_function(_));

  // Two scopes: parameters and block
  EXPECT_CALL(ids_ctx, enter_local_ctx()).Times(2);
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
       Visit_ExportedFunctionInLocalContext_ReportError)
{
  errs_t errs;
  sema_context_mock ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  auto name_token = token_identifier("bar");
  auto block = create_block_node_ptr();
  auto node =
    create_user_function_node(m_int_type_data.representation, name_token,
                              std::move(block), {}, token_kw_export());

  EXPECT_CALL(ids_ctx, is_in_global_ctx()).WillRepeatedly(Return(false));
  EXPECT_CALL(errs.mock, notify_error(_)).Times(AnyNumber());

  visitor.visit(*node);

  ASSERT_THAT(visitor.m_result_node, IsNull());
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_FunctionWithParameters_GetFunctionNodeWithParameters)
{
  errs_t errs;
  sema_context_mock ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  auto name_token = token_identifier("bar");
  auto param_name_token = token_identifier("baz");

  ast::user_function_node::params_t params;
  params.emplace_back(ast::param_declaration{ m_int_type_data.representation,
                                              param_name_token });

  auto block = create_block_node_ptr();
  auto node =
    create_user_function_node(m_int_type_data.representation, name_token,
                              std::move(block), std::move(params));

  const auto expected_found_void =
    types_context::type_with_reference{ m_void_type_data.ty,
                                        m_void_type_data.ty };
  const auto expected_found_int =
    types_context::type_with_reference{ m_int_type_data.ty,
                                        m_int_type_data.ty };
  EXPECT_CALL(types_ctx, find(m_int_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_int));
  EXPECT_CALL(types_ctx, find(m_void_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_void));

  EXPECT_CALL(functions_ctx, find_in_current_scope(_))
    .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(functions_ctx, register_function(name_token, _, _));

  EXPECT_CALL(ctx, add_function(_));

  // Two scopes: parameters and block
  EXPECT_CALL(ids_ctx, enter_local_ctx()).Times(2);
  EXPECT_CALL(ids_ctx, register_identifier(_, _, _));
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

TEST_F(SemaBuilderAstVisitorTest,
       Visit_VoidFunctionWithEmptyBody_GetFunctionNodeWithImplicitReturn)
{
  errs_t errs;
  sema_context_mock ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  auto name_token = token_identifier("foo");
  auto block = create_block_node_ptr();
  auto node = create_user_function_node(m_void_type_data.representation,
                                        name_token, std::move(block));

  const auto expected_found_type =
    types_context::type_with_reference{ m_void_type_data.ty,
                                        m_void_type_data.ty };
  EXPECT_CALL(types_ctx, find(m_void_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_type));

  EXPECT_CALL(functions_ctx, find_in_current_scope(_))
    .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(functions_ctx, register_function(name_token, _, _));

  EXPECT_CALL(ctx, add_function(_));

  // Two scopes: parameters and block
  EXPECT_CALL(ids_ctx, enter_local_ctx()).Times(2);
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(2);

  visitor.visit(*node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<function_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  const auto expected_number_of_params{ 0u };
  EXPECT_THAT(casted_node->signature().params.size(),
              Eq(expected_number_of_params));

  const auto& body = casted_node->body();
  ASSERT_THAT(body.nodes().size(), Eq(1u));

  const auto casted_last_node =
    dynamic_cast<const implicit_return_node*>(body.nodes().back().get());
  EXPECT_THAT(casted_last_node, NotNull());
}

TEST_F(
  SemaBuilderAstVisitorTest,
  Visit_VoidFunctionWithNonEmptyBodyAndWithoutReturnStatement_GetFunctionNodeWithImplicitReturn)
{
  errs_t errs;
  sema_context_mock ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  auto name_token = token_identifier("foo");
  auto block_subnode = create_block_node_ptr();
  ast::block_node::nodes_t block_nodes;
  block_nodes.emplace_back(std::move(block_subnode));
  auto block = create_block_node_ptr(std::move(block_nodes));
  auto node = create_user_function_node(m_void_type_data.representation,
                                        name_token, std::move(block));

  const auto expected_found_type =
    types_context::type_with_reference{ m_void_type_data.ty,
                                        m_void_type_data.ty };
  EXPECT_CALL(types_ctx, find(m_void_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_type));

  EXPECT_CALL(functions_ctx, find_in_current_scope(_))
    .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(functions_ctx, register_function(name_token, _, _));

  EXPECT_CALL(ctx, add_function(_));

  // Three scopes: parameters and two blocks
  EXPECT_CALL(ids_ctx, enter_local_ctx()).Times(3);
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(3);

  visitor.visit(*node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<function_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  const auto expected_number_of_params{ 0u };
  EXPECT_THAT(casted_node->signature().params.size(),
              Eq(expected_number_of_params));

  const auto& body = casted_node->body();
  ASSERT_THAT(body.nodes().size(), Eq(2u));

  const auto casted_last_node =
    dynamic_cast<const implicit_return_node*>(body.nodes().back().get());
  EXPECT_THAT(casted_last_node, NotNull());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_ClassEmpty_GetEmptyClassNode)
{
  errs_t errs;
  sema_context_mock ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  auto class_name_token = token_identifier("foo");

  ast::class_node node{ std::nullopt, tmp_token, class_name_token, tmp_token,
                        {},           tmp_token, tmp_token };

  const auto class_type_name_ref =
    ast::type_representation{ class_name_token };

  EXPECT_CALL(ctx, add_type(_)).Times(2); // Type and reference

  EXPECT_CALL(ids_ctx, enter_global_ctx(class_name_token, _));
  EXPECT_CALL(ids_ctx, leave_ctx());

  EXPECT_CALL(types_ctx, find_in_current_scope(class_name_token))
    .WillOnce(Return(nullptr));

  EXPECT_CALL(types_ctx, register_type(class_name_token, _, _)).Times(2);
  EXPECT_CALL(types_ctx, enter_global_ctx(class_name_token, _));
  EXPECT_CALL(types_ctx, leave_ctx());

  EXPECT_CALL(functions_ctx, enter_global_ctx(class_name_token, _));
  EXPECT_CALL(functions_ctx, leave_ctx());

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
  sema_context_mock ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  auto class_name_token = token_identifier("foo");
  const auto member_name_token = token_identifier("bar");

  auto member_declaration = create_standalone_variable_declaration_node(
    m_int_type_data.representation, member_name_token);
  ast::class_node::nodes_t nodes;
  nodes.emplace_back(std::move(member_declaration));

  ast::class_node node{ std::nullopt, tmp_token,        class_name_token,
                        tmp_token,    std::move(nodes), tmp_token,
                        tmp_token };

  const auto class_type_name_ref =
    ast::type_representation{ class_name_token };

  const auto expected_found_int =
    types_context::type_with_reference{ m_int_type_data.ty,
                                        m_int_type_data.ty };
  const auto expected_found_void =
    types_context::type_with_reference{ m_void_type_data.ty,
                                        m_void_type_data.ty };
  EXPECT_CALL(types_ctx, find(m_int_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_int));
  EXPECT_CALL(types_ctx, find(m_void_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_void));

  EXPECT_CALL(ctx, add_type(_)).Times(2); // Type and reference

  EXPECT_CALL(ids_ctx, enter_global_ctx(class_name_token, _));
  EXPECT_CALL(ids_ctx, register_identifier(_, _, _));
  EXPECT_CALL(ids_ctx, leave_ctx());

  EXPECT_CALL(types_ctx, find_in_current_scope(class_name_token))
    .WillOnce(Return(nullptr));
  EXPECT_CALL(types_ctx, register_type(class_name_token, _, _))
    .Times(2); // Type and reference
  EXPECT_CALL(types_ctx, enter_global_ctx(class_name_token, _));
  EXPECT_CALL(types_ctx, leave_ctx());

  EXPECT_CALL(functions_ctx, enter_global_ctx(class_name_token, _));
  EXPECT_CALL(functions_ctx, leave_ctx());

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
  sema_context_mock ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto class_name_token = token_identifier("foo");

  const auto function_name_token = token_identifier("bar");
  auto function_body = create_block_node_ptr();
  auto function =
    create_user_function_node(m_int_type_data.representation,
                              function_name_token, std::move(function_body));
  ast::class_node::nodes_t nodes;
  nodes.emplace_back(std::move(function));
  ast::class_node node{ std::nullopt, tmp_token,        class_name_token,
                        tmp_token,    std::move(nodes), tmp_token,
                        tmp_token };

  // Class type lookup
  const auto class_type_name_ref =
    ast::type_representation{ class_name_token };

  const auto expected_found_int =
    types_context::type_with_reference{ m_int_type_data.ty,
                                        m_int_type_data.ty };
  const auto expected_found_void =
    types_context::type_with_reference{ m_void_type_data.ty,
                                        m_void_type_data.ty };
  EXPECT_CALL(types_ctx, find(m_int_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_int));
  EXPECT_CALL(types_ctx, find(m_void_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_void));

  EXPECT_CALL(ctx, add_type(_)).Times(2); // Type and reference

  // There are three identifier contextes: class, function parameters and
  // function body.
  EXPECT_CALL(ids_ctx, enter_global_ctx(class_name_token, _)).Times(1);
  EXPECT_CALL(ids_ctx, enter_local_ctx()).Times(2);
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(3);

  EXPECT_CALL(types_ctx, find_in_current_scope(class_name_token))
    .WillOnce(Return(nullptr));
  EXPECT_CALL(types_ctx, register_type(class_name_token, _, _))
    .Times(2); // Type and reference
  EXPECT_CALL(types_ctx, enter_global_ctx(class_name_token, _));
  EXPECT_CALL(types_ctx, leave_ctx());

  EXPECT_CALL(functions_ctx, find_in_current_scope(_))
    .WillOnce(Return(nullptr));
  EXPECT_CALL(functions_ctx, register_function(function_name_token, _, _));
  EXPECT_CALL(functions_ctx, enter_global_ctx(class_name_token, _));
  EXPECT_CALL(functions_ctx, leave_ctx());

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
  sema_context_mock ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto class_name_token = token_identifier("foo");

  const auto member_name_token = token_identifier("baz");
  auto member_declaration = create_standalone_variable_declaration_node(
    m_int_type_data.representation, member_name_token);

  const auto function_name_token = token_identifier("bar");
  auto function_body = create_block_node_ptr();
  auto function =
    create_user_function_node(m_int_type_data.representation,
                              function_name_token, std::move(function_body));

  ast::class_node::nodes_t nodes;
  nodes.emplace_back(std::move(member_declaration));
  nodes.emplace_back(std::move(function));
  ast::class_node node{ std::nullopt, tmp_token,        class_name_token,
                        tmp_token,    std::move(nodes), tmp_token,
                        tmp_token };

  // Class type lookup.
  const auto class_type_name_ref =
    ast::type_representation{ class_name_token };

  const auto expected_found_int =
    types_context::type_with_reference{ m_int_type_data.ty,
                                        m_int_type_data.ty };
  const auto expected_found_void =
    types_context::type_with_reference{ m_void_type_data.ty,
                                        m_void_type_data.ty };
  EXPECT_CALL(types_ctx, find(m_int_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_int));
  EXPECT_CALL(types_ctx, find(m_void_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_void));

  EXPECT_CALL(ctx, add_type(_)).Times(2); // Type and reference

  // There are three identifier contextes: class, function parameters and
  // function body. Member is registered in class context.
  EXPECT_CALL(ids_ctx, enter_global_ctx(class_name_token, _)).Times(1);
  EXPECT_CALL(ids_ctx, enter_local_ctx()).Times(2);
  EXPECT_CALL(ids_ctx, register_identifier(_, _, _));
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(3);

  EXPECT_CALL(types_ctx, find_in_current_scope(class_name_token))
    .WillOnce(Return(nullptr));
  EXPECT_CALL(types_ctx, register_type(class_name_token, _, _))
    .Times(2); // Type and reference
  EXPECT_CALL(types_ctx, enter_global_ctx(class_name_token, _));
  EXPECT_CALL(types_ctx, leave_ctx());

  EXPECT_CALL(functions_ctx, find_in_current_scope(_))
    .WillOnce(Return(nullptr));
  EXPECT_CALL(functions_ctx, register_function(function_name_token, _, _));
  EXPECT_CALL(functions_ctx, enter_global_ctx(class_name_token, _));
  EXPECT_CALL(functions_ctx, leave_ctx());

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
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto condition_identifier_token = token_identifier("foo");
  auto condition_ast_node = create_ast_id_node(condition_identifier_token);
  auto body = create_block_node_ptr();
  auto conditional_ast_node = std::make_unique<ast::conditional_node>(
    tmp_token, std::move(condition_ast_node), tmp_token, std::move(body));

  ast::while_node node(tmp_token, std::move(conditional_ast_node));

  auto expected_info = identifier_info{ valid_type_data.ty, 0u };
  EXPECT_CALL(ids_ctx, info_of(_)).WillOnce(Return(expected_info));

  EXPECT_CALL(ids_ctx, enter_local_ctx()).Times(2);
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
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto condition_identifier_token = token_identifier("foo");
  auto condition_ast_node = create_ast_id_node(condition_identifier_token);
  auto body = create_block_node_ptr();
  auto conditional_ast_node = std::make_unique<ast::conditional_node>(
    tmp_token, std::move(condition_ast_node), tmp_token, std::move(body));

  ast::if_else_node::ifs_t ifs;
  ifs.emplace_back(ast::if_else_node::if_values{
    std::nullopt, tmp_token, std::move(conditional_ast_node) });

  ast::if_else_node node(std::move(ifs), std::nullopt);

  auto expected_info = identifier_info{ valid_type_data.ty, 0u };
  EXPECT_CALL(ids_ctx, info_of(_)).WillOnce(Return(expected_info));

  EXPECT_CALL(ids_ctx, enter_local_ctx()).Times(2);
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
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto condition_identifier_token = token_identifier("foo");
  auto condition_ast_node = create_ast_id_node(condition_identifier_token);
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

  auto expected_info = identifier_info{ valid_type_data.ty, 0u };
  EXPECT_CALL(ids_ctx, info_of(_)).WillOnce(Return(expected_info));

  EXPECT_CALL(ids_ctx, enter_local_ctx()).Times(4);
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
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto condition_identifier_token = token_identifier("foo");
  auto condition_ast_node = create_ast_id_node(condition_identifier_token);
  auto body = create_block_node_ptr();
  auto conditional_ast_node = std::make_unique<ast::conditional_node>(
    tmp_token, std::move(condition_ast_node), tmp_token, std::move(body));

  const auto condition_identifier_token_2 = token_identifier("bar");
  auto condition_ast_node_2 = create_ast_id_node(condition_identifier_token_2);
  auto body_2 = create_block_node_ptr();
  auto conditional_ast_node_2 = std::make_unique<ast::conditional_node>(
    tmp_token, std::move(condition_ast_node_2), tmp_token, std::move(body_2));

  ast::if_else_node::ifs_t ifs;
  ifs.emplace_back(ast::if_else_node::if_values{
    std::nullopt, tmp_token, std::move(conditional_ast_node) });
  ifs.emplace_back(ast::if_else_node::if_values{
    tmp_token, tmp_token, std::move(conditional_ast_node_2) });

  ast::if_else_node node(std::move(ifs), std::nullopt);

  auto expected_info = identifier_info{ valid_type_data.ty, 0u };
  EXPECT_CALL(ids_ctx, info_of(_)).WillRepeatedly(Return(expected_info));

  EXPECT_CALL(ids_ctx, enter_local_ctx()).Times(4);
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
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto condition_identifier_token = token_identifier("foo");
  auto condition_ast_node = create_ast_id_node(condition_identifier_token);
  auto body = create_block_node_ptr();
  auto conditional_ast_node = std::make_unique<ast::conditional_node>(
    tmp_token, std::move(condition_ast_node), tmp_token, std::move(body));

  const auto condition_identifier_token_2 = token_identifier("bar");
  auto condition_ast_node_2 = create_ast_id_node(condition_identifier_token_2);
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

  auto expected_info = identifier_info{ valid_type_data.ty, 0u };
  EXPECT_CALL(ids_ctx, info_of(_)).WillRepeatedly(Return(expected_info));

  EXPECT_CALL(ids_ctx, enter_local_ctx()).Times(6);
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
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto lhs_id_token = token_identifier("foo");
  auto lhs_node = create_ast_id_node(lhs_id_token);

  const auto member_name_token = token_identifier("bar");
  std::vector<member_info> members;
  members.emplace_back(member_info{ member_name_token, valid_type_data.ty });
  const auto lhs_type_name_token = token_identifier("baz");
  sema_type lhs_type{ ctx, ast::type_representation{ lhs_type_name_token },
                      std::move(members) };

  ast::class_member_access_node node{ std::move(lhs_node), tmp_token,
                                      member_name_token };

  auto expected_info = identifier_info{ lhs_type, 0u };
  EXPECT_CALL(ids_ctx, info_of(_)).WillOnce(Return(expected_info));

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
  sema_context_mock ctx;
  NiceMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto variable_name_token = token_identifier("foo");
  auto variable_declaration_ast_node =
    create_standalone_variable_declaration_node(m_int_type_data.representation,
                                                variable_name_token);

  const auto function_name_token = token_identifier("bar");
  auto function_body = create_block_node_ptr();
  auto function_ast_node =
    create_user_function_node(m_int_type_data.representation,
                              function_name_token, std::move(function_body));

  const auto class_name_token = token_identifier("baz");
  auto class_ast_node = std::make_unique<ast::class_node>(
    std::nullopt, tmp_token, class_name_token, tmp_token,
    ast::class_node::nodes_t{}, tmp_token, tmp_token);

  ast::translation_unit_node::nodes_t nodes;
  nodes.emplace_back(std::move(variable_declaration_ast_node));
  nodes.emplace_back(std::move(function_ast_node));
  nodes.emplace_back(std::move(class_ast_node));

  ast::translation_unit_node node{ std::move(nodes) };

  const auto expected_found_type =
    types_context::type_with_reference{ m_int_type_data.ty,
                                        m_int_type_data.ty };
  EXPECT_CALL(types_ctx, find(m_int_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_type));

  const auto class_type_name_ref =
    ast::type_representation{ class_name_token };

  EXPECT_CALL(ctx, add_function(_));
  EXPECT_CALL(ctx, add_type(_)).Times(2); // Type and reference

  EXPECT_CALL(types_ctx, find_in_current_scope(class_name_token))
    .WillOnce(Return(nullptr));
  EXPECT_CALL(types_ctx, register_type(class_name_token, _, _))
    .Times(2); // Type and reference
  EXPECT_CALL(types_ctx, enter_global_ctx(class_name_token, _));
  EXPECT_CALL(types_ctx, leave_ctx());

  EXPECT_CALL(functions_ctx, find_in_current_scope(_))
    .WillOnce(Return(nullptr));
  EXPECT_CALL(functions_ctx, register_function(function_name_token, _, _));
  EXPECT_CALL(functions_ctx, enter_global_ctx(class_name_token, _));
  EXPECT_CALL(functions_ctx, leave_ctx());

  EXPECT_CALL(ids_ctx, register_identifier(_, _, _));

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
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const sema_type lhs_and_rhs_type{ ctx,
                                    ast::type_representation{ token_kw_int() },
                                    {} };
  const auto lhs_id_token = token_identifier("foo");
  auto lhs_node = create_ast_id_node(lhs_id_token);
  const auto rhs_id_token = token_identifier("bar");
  auto rhs_node = create_ast_id_node(rhs_id_token);

  const auto operator_token = token_plus();

  ast::binary_operator_node node{ std::move(lhs_node), operator_token,
                                  std::move(rhs_node) };

  auto expected_info = identifier_info{ lhs_and_rhs_type, 0u };
  EXPECT_CALL(ids_ctx, info_of(_)).WillRepeatedly(Return(expected_info));

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

TEST_F(
  SemaBuilderAstVisitorTest,
  Visit_ForLoopWithoutInitWithoutConditionWithoutIteration_GetForWithoutInitWithoutConditionWithoutIteration)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  auto body = create_block_node_ptr();

  auto node = ast::for_node{ token_kw_for(), token_open_paren(),
                             nullptr,        token_semicolon(),
                             nullptr,        token_semicolon(),
                             nullptr,        token_close_paren(),
                             std::move(body) };

  // One ids context covering initialization and one for body.
  EXPECT_CALL(ids_ctx, enter_local_ctx()).Times(2);
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(2);

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<for_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
  EXPECT_THAT(casted_node->init(), IsNull());
  EXPECT_THAT(casted_node->condition(), IsNull());
  EXPECT_THAT(casted_node->iteration(), IsNull());
}

TEST_F(
  SemaBuilderAstVisitorTest,
  Visit_ForLoopWithInitWithConditionWithIteration_GetForWithInitWithConditionWithIteration)
{
  errs_t errs;
  sema_context_mock ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<sema_function_mock> function_mock;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto variable_name_token = token_identifier("foo");
  auto var_decl = std::make_unique<ast::variable_declaration_node>(
    std::nullopt, m_int_type_data.representation, variable_name_token,
    std::nullopt);

  auto condition = std::make_unique<ast::bool_value_node>(token_kw_true());

  const auto fun_name_token = token_identifier("fun");
  const auto qualified_fun_name = create_qualified_name(fun_name_token);
  auto iteration = create_function_call_ptr(fun_name_token);

  auto body = create_block_node_ptr();

  auto node = ast::for_node{ token_kw_for(),       token_open_paren(),
                             std::move(var_decl),  token_semicolon(),
                             std::move(condition), token_semicolon(),
                             std::move(iteration), token_close_paren(),
                             std::move(body) };

  // One ids context covering initialization and one for body.
  EXPECT_CALL(ids_ctx, enter_local_ctx()).Times(2);
  EXPECT_CALL(ids_ctx, leave_ctx()).Times(2);
  EXPECT_CALL(ids_ctx, register_identifier(_, _, _));

  EXPECT_CALL(ctx, type())
    .WillRepeatedly(Return(sema_context::context_type::namespace_));

  const auto expected_found_type =
    types_context::type_with_reference{ m_int_type_data.ty,
                                        m_int_type_data.ty };
  EXPECT_CALL(types_ctx, find(m_int_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_type));

  EXPECT_CALL(types_ctx, find(qualified_fun_name))
    .WillRepeatedly(Return(std::nullopt));

  const auto lookup_result = function_lookup_result_t{ { &function_mock } };
  EXPECT_CALL(functions_ctx, find(qualified_fun_name))
    .WillOnce(Return(lookup_result));

  function_signature signature;
  signature.name = fun_name_token;
  EXPECT_CALL(function_mock, signature()).WillRepeatedly(ReturnRef(signature));

  EXPECT_CALL(function_mock, return_type())
    .WillRepeatedly(ReturnRef(valid_type_data.ty));
  EXPECT_CALL(function_mock, try_return_type())
    .WillRepeatedly(Return(&valid_type_data.ty));

  EXPECT_CALL(function_mock, context()).WillRepeatedly(ReturnRef(ctx));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<for_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());

  ASSERT_THAT(casted_node->init(), NotNull());
  const auto casted_init =
    dynamic_cast<const variable_declaration_node*>(casted_node->init());
  EXPECT_THAT(casted_init, NotNull());

  EXPECT_THAT(casted_node->condition(), NotNull());
  EXPECT_THAT(casted_node->iteration(), NotNull());
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_BlockWithBreakNotInsideLoop_ReportError)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  NiceMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  auto break_ =
    std::make_unique<ast::break_node>(token_kw_break(), token_semicolon());

  ast::block_node::nodes_t nodes;
  nodes.emplace_back(std::move(break_));

  auto block = create_block_node_ptr(std::move(nodes));

  EXPECT_CALL(errs.mock, notify_error(_)).Times(AnyNumber());

  visitor.visit(*block);

  ASSERT_THAT(visitor.m_result_node, IsNull());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_TernaryOperator_GetTernaryOperator)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  NiceMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  auto condition = std::make_unique<ast::bool_value_node>(token_kw_false());
  auto true_ = std::make_unique<ast::int_value_node>(token_integer("12"));
  auto false_ = std::make_unique<ast::int_value_node>(token_integer("42"));

  auto ternary = std::make_unique<ast::ternary_operator_node>(
    std::move(condition), token_question(), std::move(true_), token_colon(),
    std::move(false_));

  EXPECT_CALL(ctx, find_type(_)).WillRepeatedly(Return(&valid_type_data.ty));
  EXPECT_CALL(ctx, find_reference_for(_))
    .WillRepeatedly(Return(&valid_type_reference));

  visitor.visit(*ternary);

  ASSERT_THAT(visitor.m_result_node, NotNull());
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_TernaryOperatorWithConditionNotReturningBool_ReportError)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  NiceMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  auto condition = std::make_unique<ast::int_value_node>(token_integer("12"));
  auto true_ = std::make_unique<ast::int_value_node>(token_integer("12"));
  auto false_ = std::make_unique<ast::int_value_node>(token_integer("42"));

  auto ternary = std::make_unique<ast::ternary_operator_node>(
    std::move(condition), token_question(), std::move(true_), token_colon(),
    std::move(false_));

  const auto expected_found_bool =
    types_context::type_with_reference{ m_bool_type_data.ty,
                                        m_bool_type_data.ty };
  const auto expected_found_int =
    types_context::type_with_reference{ m_int_type_data.ty,
                                        m_int_type_data.ty };
  EXPECT_CALL(types_ctx, find(m_int_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_int));
  EXPECT_CALL(types_ctx, find(m_bool_type_data.qualified_names))
    .WillRepeatedly(Return(expected_found_bool));

  EXPECT_CALL(errs.mock, notify_error(_)).Times(AnyNumber());

  visitor.visit(*ternary);

  ASSERT_THAT(visitor.m_result_node, IsNull());
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_TernaryOperatorWithArgumentsReturningDifferentTypes_ReportError)
{
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  NiceMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  auto condition = std::make_unique<ast::bool_value_node>(token_kw_false());
  auto true_ =
    std::make_unique<ast::string_value_node>(token_string("some_str"));
  auto false_ = std::make_unique<ast::int_value_node>(token_integer("42"));

  auto ternary = std::make_unique<ast::ternary_operator_node>(
    std::move(condition), token_question(), std::move(true_), token_colon(),
    std::move(false_));

  EXPECT_CALL(errs.mock, notify_error(_)).Times(AnyNumber());

  visitor.visit(*ternary);

  ASSERT_THAT(visitor.m_result_node, IsNull());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_UnaryOperatorNode)
{
  // -foo
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<sema_function_mock> operator_function;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const sema_type foo_type{
    ctx, ast::type_representation{ token_identifier("foo_type") }, {}
  };
  const auto foo_token = token_identifier("foo");
  auto foo_node = create_ast_id_node(foo_token);

  const auto operator_token = token_minus();

  ast::unary_operator node{ operator_token, std::move(foo_node) };

  auto expected_info = identifier_info{ foo_type, 0u };
  EXPECT_CALL(ids_ctx, info_of(_)).WillRepeatedly(Return(expected_info));

  const auto expected_signature = function_signature{ operator_token, {} };
  EXPECT_CALL(operator_function, signature())
    .WillOnce(ReturnRef(expected_signature));

  EXPECT_CALL(operator_function, return_type()).WillOnce(ReturnRef(foo_type));

  // Find operator member function.
  const auto lookup_result =
    single_scope_function_lookup_result_t{ &operator_function };
  EXPECT_CALL(ctx, find_function_in_this_scope(operator_token))
    .WillOnce(Return(lookup_result));

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<unary_operator_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
  EXPECT_THAT(&casted_node->type(), Eq(&foo_type));
}

TEST_F(SemaBuilderAstVisitorTest, Visit_Enum)
{
  // enum foo{ bar, baz };
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto foo_token = token_identifier("foo");
  const auto bar_token = token_identifier("bar");
  const auto baz_token = token_identifier("baz");

  ast::enum_node node{
    std::nullopt,       token_kw_enum(),          foo_token,
    token_open_brace(), { bar_token, baz_token }, token_close_brace(),
    token_semicolon()
  };

  EXPECT_CALL(types_ctx, find_in_current_scope(_))
    .WillRepeatedly(Return(nullptr));

  EXPECT_CALL(types_ctx, register_type(_, _, _))
    .Times(2); // Enum type and its reference

  EXPECT_CALL(ctx, add_type(_)).Times(2); // Enum type and its reference

  EXPECT_CALL(functions_ctx, enter_global_ctx(foo_token, _));
  EXPECT_CALL(functions_ctx, leave_ctx());
  EXPECT_CALL(enums_ctx, enter_global_ctx(foo_token, _));
  EXPECT_CALL(enums_ctx, register_identifier(bar_token, _, _));
  EXPECT_CALL(enums_ctx, register_identifier(baz_token, _, _));
  EXPECT_CALL(enums_ctx, leave_ctx());

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, NotNull());

  const auto casted_node =
    dynamic_cast<enum_node*>(visitor.m_result_node.get());
  ASSERT_THAT(casted_node, NotNull());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_Enum_TypeRedefinition_ReportError)
{
  // enum foo{};
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto foo_token = token_identifier("foo");

  ast::enum_node node{
    std::nullopt, token_kw_enum(),     foo_token,        token_open_brace(),
    {},           token_close_brace(), token_semicolon()
  };

  EXPECT_CALL(types_ctx, find_in_current_scope(_))
    .WillRepeatedly(Return(&valid_type_data.ty));

  // It reports error and a note.
  EXPECT_CALL(errs.mock, notify_error(_)).Times(AnyNumber());

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, IsNull());
}

TEST_F(SemaBuilderAstVisitorTest, Visit_Import_HandlerFailed_ReportError)
{
  // import "foo"
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto foo_token = token_string("\"foo\"");

  ast::import_node node{ token_kw_import(), foo_token };

  EXPECT_CALL(m_import_handler_mock, handle_import(_))
    .WillRepeatedly(Return(ByMove(nullptr)));

  EXPECT_CALL(errs.mock, notify_error(_)).Times(AnyNumber());

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, IsNull());
}

TEST_F(SemaBuilderAstVisitorTest,
       Visit_Import_MergingEntriesFailed_ReportError)
{
  // import "foo"
  errs_t errs;
  StrictMock<sema_context_mock> ctx;
  StrictMock<identifiers_context_mock> ids_ctx;
  StrictMock<types_context_mock> types_ctx;
  StrictMock<functions_context_mock> functions_ctx;
  enum_values_context_mock enums_ctx;
  auto [visitor_members, visitor] = create_types_factory_and_visitor(
    errs, ctx, enums_ctx, ids_ctx, types_ctx, functions_ctx);

  const auto foo_token = token_string("\"foo\"");

  ast::import_node node{ token_kw_import(), foo_token };

  auto qualified =
    qualified_contextes{ std::make_unique<enum_values_context_mock>(),
                         std::make_unique<functions_context_mock>(),
                         std::make_unique<identifiers_context_mock>(),
                         std::make_unique<types_context_mock>() };
  auto qualified_ptr =
    std::make_unique<qualified_contextes>(std::move(qualified));

  EXPECT_CALL(m_import_handler_mock, handle_import(_))
    .WillRepeatedly(Return(ByMove(std::move(qualified_ptr))));

  EXPECT_CALL(enums_ctx, merge_imported_stuff(_, _)).WillOnce(Return(false));
  EXPECT_CALL(functions_ctx, merge_imported_stuff(_, _))
    .WillOnce(Return(true));
  EXPECT_CALL(ids_ctx, merge_imported_stuff(_, _)).WillOnce(Return(true));
  EXPECT_CALL(types_ctx, merge_imported_stuff(_, _)).WillOnce(Return(true));

  EXPECT_CALL(errs.mock, notify_error(_)).Times(AnyNumber());

  visitor.visit(node);

  ASSERT_THAT(visitor.m_result_node, IsNull());
}
}
