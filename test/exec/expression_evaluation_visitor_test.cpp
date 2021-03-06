#include "exec/expression_evaluation_visitor.hpp"

#include "sema/sema_context_impl.hpp"
#include "sema/sema_type.hpp"

#include "test/ast/mock/ast_node_mock.hpp"
#include "test/common/tokens.hpp"
#include "test/exec/mock/function_caller_mock.hpp"
#include "test/exec/mock/identifiers_context_mock.hpp"
#include "test/exec/mock/instance_mock.hpp"
#include "test/exec/mock/instances_holder_mock.hpp"
#include "test/mock/cmake_facade_mock.hpp"
#include "test/sema/mock/expression_node_mock.hpp"
#include "test/sema/mock/sema_function_mock.hpp"

#include "common/int_alias.hpp"
#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::_;
using ::testing::ByRef;
using ::testing::ByMove;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::Eq;
using ::testing::StrictMock;
using ::testing::NiceMock;
using ::testing::Matcher;

using namespace cmsl::test::common;

const sema::sema_context_impl valid_context{ "" };
const sema::sema_type valid_type{ valid_context,
                                  ast::type_representation{
                                    token_identifier("foo") },
                                  {} };

class ExpressionEvaluationVisitorTest : public ::testing::Test
{
protected:
  auto fake_ast_node() { return NiceMock<ast::test::ast_node_mock>{}; }

  StrictMock<identifiers_context_mock> m_ids_ctx;
  StrictMock<function_caller2_mock> m_caller;
  StrictMock<inst::test::instances_holder_mock> m_instances;
  StrictMock<cmake_facade_mock> m_cmake_facade;
  expression_evaluation_context m_ctx{ m_caller, m_instances, m_ids_ctx,
                                       m_cmake_facade };

  std::vector<ast::name_with_coloncolon> create_qualified_name(token_t name)
  {
    return { { name } };
  }

  std::unique_ptr<sema::id_node> create_id_node_ptr(
    const ast::ast_node& ast_node, token_t name, unsigned index)
  {
    return std::make_unique<sema::id_node>(ast_node, valid_type,
                                           create_qualified_name(name), index);
  }
};

// Todo: move to a common file.
MATCHER_P(MatchRef, ptr, "")
{
  return &arg == ptr;
}

TEST_F(ExpressionEvaluationVisitorTest,
       Visit_BoolValue_CreatesInstanceAndStoresAsResult)
{
  StrictMock<inst::test::instance_mock> instance_mock;
  expression_evaluation_visitor visitor{ m_ctx };

  // Todo: consider testing false too.
  auto ast_node = fake_ast_node();
  sema::bool_value_node true_node{ ast_node, valid_type, true };
  inst::instance_value_variant value{ true };

  EXPECT_CALL(m_instances, create(Matcher<inst::instance_value_variant>(_)))
    .WillOnce(Return(&instance_mock));

  EXPECT_CALL(m_cmake_facade, did_fatal_error_occure())
    .WillRepeatedly(Return(false));

  visitor.visit(true_node);

  EXPECT_THAT(visitor.result, Eq(&instance_mock));
}

TEST_F(ExpressionEvaluationVisitorTest,
       Visit_IntValue_CreatesInstanceAndStoresAsResult)
{
  StrictMock<inst::test::instance_mock> instance_mock;
  expression_evaluation_visitor visitor{ m_ctx };

  auto ast_node = fake_ast_node();
  const auto test_value = int_t{ 42 };
  sema::int_value_node node{ ast_node, valid_type, test_value };
  inst::instance_value_variant value{ test_value };

  EXPECT_CALL(m_instances, create(Matcher<inst::instance_value_variant>(_)))
    .WillOnce(Return(&instance_mock));

  EXPECT_CALL(m_cmake_facade, did_fatal_error_occure())
    .WillRepeatedly(Return(false));

  visitor.visit(node);

  EXPECT_THAT(visitor.result, Eq(&instance_mock));
}

TEST_F(ExpressionEvaluationVisitorTest,
       Visit_DoubleValue_CreatesInstanceAndStoresAsResult)
{
  StrictMock<inst::test::instance_mock> instance_mock;
  expression_evaluation_visitor visitor{ m_ctx };

  auto ast_node = fake_ast_node();
  const auto test_value = 42.42;
  sema::double_value_node node{ ast_node, valid_type, test_value };
  inst::instance_value_variant value{ test_value };

  EXPECT_CALL(m_instances, create(Matcher<inst::instance_value_variant>(_)))
    .WillOnce(Return(&instance_mock));

  EXPECT_CALL(m_cmake_facade, did_fatal_error_occure())
    .WillRepeatedly(Return(false));

  visitor.visit(node);

  EXPECT_THAT(visitor.result, Eq(&instance_mock));
}

TEST_F(ExpressionEvaluationVisitorTest,
       Visit_StringValue_CreatesInstanceAndStoresAsResult)
{
  StrictMock<inst::test::instance_mock> instance_mock;
  expression_evaluation_visitor visitor{ m_ctx };

  auto ast_node = fake_ast_node();
  const auto test_value = std::string{ "42" };
  sema::string_value_node node{ ast_node, valid_type, test_value };
  inst::instance_value_variant value{ test_value };

  EXPECT_CALL(m_instances, create(Matcher<inst::instance_value_variant>(_)))
    .WillOnce(Return(&instance_mock));

  EXPECT_CALL(m_cmake_facade, did_fatal_error_occure())
    .WillRepeatedly(Return(false));

  visitor.visit(node);

  EXPECT_THAT(visitor.result, Eq(&instance_mock));
}

TEST_F(ExpressionEvaluationVisitorTest,
       Visit_Identifier_CreatesInstanceAndStoresAsResult)
{
  StrictMock<inst::test::instance_mock> instance_mock;
  expression_evaluation_visitor visitor{ m_ctx };

  auto ast_node = fake_ast_node();
  const auto id_token = token_identifier("foo");
  const auto identifier_index = 0u;
  sema::id_node node{
    ast_node, valid_type, { { id_token } }, identifier_index
  };

  EXPECT_CALL(m_ids_ctx, lookup_identifier(identifier_index))
    .WillOnce(Return(&instance_mock));

  EXPECT_CALL(m_cmake_facade, did_fatal_error_occure())
    .WillRepeatedly(Return(false));

  visitor.visit(node);

  EXPECT_THAT(visitor.result, Eq(&instance_mock));
}

TEST_F(
  ExpressionEvaluationVisitorTest,
  Visit_BinaryOperator_EvaluatesLhsAndRhsAndCallsLhsMethodWithRhsAsAParameter)
{
  StrictMock<inst::test::instance_mock> lhs_instance;
  StrictMock<inst::test::instance_mock> rhs_instance;
  StrictMock<sema::test::sema_function_mock> function;
  auto ast_node = fake_ast_node();
  auto result_instance =
    std::make_unique<StrictMock<inst::test::instance_mock>>();
  auto result_instance_ptr = result_instance.get();

  const auto operator_token = token_plus();

  const auto lhs_expression_token = token_identifier("foo");
  const auto lhs_identifier_index = 0u;
  auto lhs_expression = std::make_unique<sema::id_node>(
    ast_node, valid_type, create_qualified_name(lhs_expression_token),
    lhs_identifier_index);
  const auto rhs_expression_token = token_identifier("bar");
  const auto rhs_identifier_index = 0u;
  auto rhs_expression = std::make_unique<sema::id_node>(
    ast_node, valid_type, create_qualified_name(rhs_expression_token),
    rhs_identifier_index);

  sema::binary_operator_node node{
    ast_node, std::move(lhs_expression), operator_token,
    function, std::move(rhs_expression), valid_type
  };

  // Evaluation of lhs expression.
  EXPECT_CALL(m_ids_ctx, lookup_identifier(_))
    .WillOnce(Return(&lhs_instance))
    .WillOnce(Return(&rhs_instance));

  const std::vector<inst::instance*> expected_param_instances = {
    &rhs_instance
  };
  EXPECT_CALL(m_caller,
              call_member(MatchRef(&lhs_instance), MatchRef(&function),
                          expected_param_instances, _))
    .WillOnce(Return(ByMove(std::move(result_instance))));

  // Function return value storing in our instances.
  EXPECT_CALL(m_instances, store(_));

  EXPECT_CALL(m_cmake_facade, did_fatal_error_occure())
    .WillRepeatedly(Return(false));

  expression_evaluation_visitor visitor{ m_ctx };

  visitor.visit(node);

  EXPECT_THAT(visitor.result, Eq(result_instance_ptr));
}

TEST_F(ExpressionEvaluationVisitorTest,
       Visit_FunctionCall_EvaluatesParametersCallsFunctionAndStoresResult)
{
  StrictMock<sema::test::sema_function_mock> function;
  StrictMock<inst::test::instance_mock> param_instance_0;
  StrictMock<inst::test::instance_mock> param_instance_1;
  auto ast_node = fake_ast_node();
  auto result_instance =
    std::make_unique<StrictMock<inst::test::instance_mock>>();
  auto result_instance_ptr = result_instance.get();

  const auto fun_name_token = token_identifier("foo");

  const auto param_expression_0_token = token_identifier("bar");
  auto param_expression_0 =
    create_id_node_ptr(ast_node, param_expression_0_token, 0u);
  const auto param_expression_1_token = token_identifier("baz");
  auto param_expression_1 =
    create_id_node_ptr(ast_node, param_expression_1_token, 1u);

  sema::function_call_node::param_expressions_t param_expressions;
  param_expressions.emplace_back(std::move(param_expression_0));
  param_expressions.emplace_back(std::move(param_expression_1));
  sema::function_call_node node{ ast_node, function,
                                 std::move(param_expressions),
                                 fun_name_token };

  EXPECT_CALL(m_ids_ctx, lookup_identifier(_))
    .WillOnce(Return(&param_instance_0))
    .WillOnce(Return(&param_instance_1));

  sema::function_signature signature;
  signature.name = fun_name_token;
  signature.params.emplace_back(
    sema::parameter_declaration{ valid_type, param_expression_0_token, 3u });
  signature.params.emplace_back(
    sema::parameter_declaration{ valid_type, param_expression_1_token, 4u });
  EXPECT_CALL(function, signature()).WillRepeatedly(ReturnRef(signature));

  std::vector<inst::instance*> expected_param_instances{ &param_instance_0,
                                                         &param_instance_1 };
  EXPECT_CALL(m_caller, call(MatchRef(&function), expected_param_instances, _))
    .WillOnce(Return(ByMove(std::move(result_instance))));

  // Function return value storing in our instances.
  EXPECT_CALL(m_instances, store(_));

  EXPECT_CALL(m_cmake_facade, did_fatal_error_occure())
    .WillRepeatedly(Return(false));

  expression_evaluation_visitor visitor{ m_ctx };
  visitor.visit(node);

  EXPECT_THAT(visitor.result, Eq(result_instance_ptr));
}

TEST_F(
  ExpressionEvaluationVisitorTest,
  Visit_MemberFunctionCall_EvaluatesLhsEvaluatesParametersCallsFunctionAndStoresResult)
{
  StrictMock<inst::test::instance_mock> lhs_instance;
  StrictMock<inst::test::instance_mock> param_instance_0;
  StrictMock<inst::test::instance_mock> param_instance_1;
  StrictMock<sema::test::sema_function_mock> function;
  auto ast_node = fake_ast_node();
  auto result_instance =
    std::make_unique<StrictMock<inst::test::instance_mock>>();
  auto result_instance_ptr = result_instance.get();

  const auto lhs_expression_token = token_identifier("foo");
  auto lhs_expression = create_id_node_ptr(ast_node, lhs_expression_token, 0u);

  const auto fun_name_token = token_identifier("bar");

  const auto param_expression_0_token = token_identifier("baz");
  auto param_expression_0 =
    create_id_node_ptr(ast_node, param_expression_0_token, 1u);
  const auto param_expression_1_token = token_identifier("qux");
  auto param_expression_1 =
    create_id_node_ptr(ast_node, param_expression_1_token, 2u);

  sema::function_call_node::param_expressions_t param_expressions;
  param_expressions.emplace_back(std::move(param_expression_0));
  param_expressions.emplace_back(std::move(param_expression_1));

  sema::member_function_call_node node{ ast_node, std::move(lhs_expression),
                                        function, std::move(param_expressions),
                                        fun_name_token };

  sema::function_signature signature;
  signature.name = fun_name_token;
  signature.params.emplace_back(
    sema::parameter_declaration{ valid_type, param_expression_0_token, 3u });
  signature.params.emplace_back(
    sema::parameter_declaration{ valid_type, param_expression_1_token, 4u });
  EXPECT_CALL(function, signature()).WillRepeatedly(ReturnRef(signature));

  EXPECT_CALL(m_ids_ctx, lookup_identifier(_))
    .WillOnce(Return(&lhs_instance))
    .WillOnce(Return(&param_instance_0))
    .WillOnce(Return(&param_instance_1));

  std::vector<inst::instance*> expected_param_instances{ &param_instance_0,
                                                         &param_instance_1 };
  EXPECT_CALL(m_caller,
              call_member(MatchRef(&lhs_instance), MatchRef(&function),
                          expected_param_instances, _))
    .WillOnce(Return(ByMove(std::move(result_instance))));

  // Function return value storing in our instances.
  EXPECT_CALL(m_instances, store(_));

  EXPECT_CALL(m_cmake_facade, did_fatal_error_occure())
    .WillRepeatedly(Return(false));

  expression_evaluation_visitor visitor{ m_ctx };
  visitor.visit(node);

  EXPECT_THAT(visitor.result, Eq(result_instance_ptr));
}

TEST_F(
  ExpressionEvaluationVisitorTest,
  Visit_ImplicitMemberFunctionCall_EvaluatesLhsEvaluatesParametersCallsFunctionAndStoresResult)
{
  StrictMock<inst::test::instance_mock> lhs_instance;
  StrictMock<inst::test::instance_mock> param_instance_0;
  StrictMock<inst::test::instance_mock> param_instance_1;
  StrictMock<sema::test::sema_function_mock> function;
  auto ast_node = fake_ast_node();
  auto result_instance =
    std::make_unique<StrictMock<inst::test::instance_mock>>();
  auto result_instance_ptr = result_instance.get();

  const auto lhs_expression_token = token_identifier("this");
  auto lhs_expression = create_id_node_ptr(ast_node, lhs_expression_token, 1u);

  const auto fun_name_token = token_identifier("bar");

  const auto param_expression_0_token = token_identifier("baz");
  auto param_expression_0 =
    create_id_node_ptr(ast_node, param_expression_0_token, 1u);
  const auto param_expression_1_token = token_identifier("qux");
  auto param_expression_1 =
    create_id_node_ptr(ast_node, param_expression_1_token, 2u);

  sema::function_call_node::param_expressions_t param_expressions;
  param_expressions.emplace_back(std::move(param_expression_0));
  param_expressions.emplace_back(std::move(param_expression_1));

  sema::member_function_call_node node{ ast_node, std::move(lhs_expression),
                                        function, std::move(param_expressions),
                                        fun_name_token };

  sema::function_signature signature;
  signature.name = fun_name_token;
  signature.params.emplace_back(
    sema::parameter_declaration{ valid_type, param_expression_0_token, 3u });
  signature.params.emplace_back(
    sema::parameter_declaration{ valid_type, param_expression_1_token, 4u });
  EXPECT_CALL(function, signature()).WillRepeatedly(ReturnRef(signature));

  // Evaluation of lhs expression.
  EXPECT_CALL(m_ids_ctx, lookup_identifier(_))
    .WillOnce(Return(&lhs_instance))
    .WillOnce(Return(&param_instance_0))
    .WillOnce(Return(&param_instance_1));

  std::vector<inst::instance*> expected_param_instances{ &param_instance_0,
                                                         &param_instance_1 };
  EXPECT_CALL(m_caller,
              call_member(MatchRef(&lhs_instance), MatchRef(&function),
                          expected_param_instances, _))
    .WillOnce(Return(ByMove(std::move(result_instance))));

  // Function return value storing in our instances.
  EXPECT_CALL(m_instances, store(_));

  EXPECT_CALL(m_cmake_facade, did_fatal_error_occure())
    .WillRepeatedly(Return(false));

  expression_evaluation_visitor visitor{ m_ctx };
  visitor.visit(node);

  EXPECT_THAT(visitor.result, Eq(result_instance_ptr));
}

TEST_F(ExpressionEvaluationVisitorTest,
       Visit_ClassMemberAccess_EvaluatesLhsGetsMemberAndStoresResult)
{
  StrictMock<inst::test::instance_mock> lhs_instance;
  StrictMock<inst::test::instance_mock> member_instance;
  auto ast_node = fake_ast_node();

  const auto lhs_expression_token = token_identifier("foo");
  auto lhs_expression = create_id_node_ptr(ast_node, lhs_expression_token, 0u);

  const auto member_name_token = token_identifier("bar");
  sema::member_info member{ member_name_token, valid_type };

  sema::class_member_access_node node{ ast_node, std::move(lhs_expression),
                                       member_name_token, std::move(member) };

  EXPECT_CALL(m_ids_ctx, lookup_identifier(_)).WillOnce(Return(&lhs_instance));

  EXPECT_CALL(lhs_instance, find_member(_)).WillOnce(Return(&member_instance));

  EXPECT_CALL(m_cmake_facade, did_fatal_error_occure())
    .WillRepeatedly(Return(false));

  expression_evaluation_visitor visitor{ m_ctx };
  visitor.visit(node);

  EXPECT_THAT(visitor.result, Eq(&member_instance));
}

TEST_F(ExpressionEvaluationVisitorTest,
       Visit_Return_EvaluatesExpressionAndStoresResult)
{
  StrictMock<inst::test::instance_mock> result_instance;
  auto ast_node = fake_ast_node();

  const auto expression_token = token_identifier("foo");
  auto expression = create_id_node_ptr(ast_node, expression_token, 0u);

  sema::return_node node{ ast_node, std::move(expression) };

  EXPECT_CALL(m_ids_ctx, lookup_identifier(_))
    .WillOnce(Return(&result_instance));

  EXPECT_CALL(m_cmake_facade, did_fatal_error_occure())
    .WillRepeatedly(Return(false));

  expression_evaluation_visitor visitor{ m_ctx };
  visitor.visit(node);

  EXPECT_THAT(visitor.result, Eq(&result_instance));
}
}
