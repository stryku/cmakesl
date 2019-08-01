#include "sema/sema_context_impl.hpp"
#include "sema/sema_nodes.hpp"
#include "sema/variable_initialization_checker.hpp"

#include "test/ast/mock/ast_node_mock.hpp"
#include "test/common/tokens.hpp"
#include "test/sema/mock/expression_node_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Eq;
using ::testing::_;

using namespace cmsl::test::common;

const sema_context_impl valid_context;
const sema_type valid_type{ valid_context,
                            ast::type_representation{
                              token_identifier("foo") },
                            {} };
const sema_type different_type{ valid_context,
                                ast::type_representation{
                                  token_identifier("bar") },
                                {} };
const sema_type designated_initialization_type{
  sema_type::designated_initializer_tag{}, valid_context,
  ast::type_representation{ token_identifier("designated_type") }
};

const sema_type complex_type{
  valid_context,
  ast::type_representation{ token_identifier("foo") },
  { member_info{ token_identifier("member_foo"), valid_type, 0 } }
};

// using check_result_t = variable_initialization_checker::check_result;

using namespace variable_initialization_issues;

class VariableInitializationCheck : public ::testing::Test
{
public:
  auto init_expression_mock()
  {
    static auto ast_node = NiceMock<ast::test::ast_node_mock>{};
    return StrictMock<expression_node_mock>{ ast_node };
  }

  auto expression_mock_ptr()
  {
    static auto ast_node = NiceMock<ast::test::ast_node_mock>{};
    return std::make_unique<StrictMock<expression_node_mock>>(ast_node);
  }

  auto designated_init_expression_mock(
    designated_initializers_node::initializers_t inits = {})
  {
    static auto ast_node = NiceMock<ast::test::ast_node_mock>{};
    return designated_initializers_node{ ast_node,
                                         designated_initialization_type,
                                         std::move(inits) };
  }

  auto member_initializer(lexer::token name)
  {
    auto member_init = expression_mock_ptr();
    const auto init_mock_ptr = member_init.get();
    return std::make_pair(
      designated_initializers_node::initializer{ name,
                                                 std::move(member_init) },
      init_mock_ptr);
  }
};

TEST_F(VariableInitializationCheck,
       NonReferenceInitializedFromNonReference_ReturnsCanInitialize)
{
  auto init_expression = init_expression_mock();

  EXPECT_CALL(init_expression, type()).WillOnce(ReturnRef(valid_type));

  variable_initialization_checker checker;
  const auto init_issues = checker.check(valid_type, init_expression);

  EXPECT_TRUE(init_issues.empty());
}

TEST_F(VariableInitializationCheck,
       NonReferenceInitializedFromReference_ReturnsCanInitialize)
{
  auto init_expression = init_expression_mock();

  sema_type expression_type{ sema_type_reference{ valid_type } };

  EXPECT_CALL(init_expression, type())
    .WillRepeatedly(ReturnRef(expression_type));

  variable_initialization_checker checker;
  const auto init_issues = checker.check(valid_type, init_expression);

  EXPECT_TRUE(init_issues.empty());
}

TEST_F(VariableInitializationCheck,
       ReferenceInitializedFromReference_ReturnsCanInitialize)
{
  auto init_expression = init_expression_mock();

  sema_type variable_type{ sema_type_reference{ valid_type } };
  sema_type expression_type{ sema_type_reference{ valid_type } };

  EXPECT_CALL(init_expression, type())
    .WillRepeatedly(ReturnRef(expression_type));

  variable_initialization_checker checker;
  const auto init_issues = checker.check(variable_type, init_expression);

  EXPECT_TRUE(init_issues.empty());
}

TEST_F(
  VariableInitializationCheck,
  ReferenceInitializedFromNonReferenceNonTemporaryValue_ReturnsCanInitialize)
{
  auto init_expression = init_expression_mock();

  sema_type variable_type{ sema_type_reference{ valid_type } };

  EXPECT_CALL(init_expression, type()).WillRepeatedly(ReturnRef(valid_type));

  EXPECT_CALL(init_expression, produces_temporary_value())
    .WillRepeatedly(Return(false));

  variable_initialization_checker checker;
  const auto init_issues = checker.check(variable_type, init_expression);

  EXPECT_TRUE(init_issues.empty());
  ;
}

TEST_F(
  VariableInitializationCheck,
  ReferenceInitializedFromNonReferenceTemporaryValue_ReturnsReferenceInitFromTemporaryValue)
{
  auto init_expression = init_expression_mock();

  sema_type variable_type{ sema_type_reference{ valid_type } };

  EXPECT_CALL(init_expression, type()).WillRepeatedly(ReturnRef(valid_type));

  EXPECT_CALL(init_expression, produces_temporary_value())
    .WillRepeatedly(Return(true));

  variable_initialization_checker checker;
  const auto init_issues = checker.check(variable_type, init_expression);

  ASSERT_THAT(init_issues.size(), Eq(1));
  EXPECT_TRUE(std::holds_alternative<reference_init_from_temporary_value>(
    init_issues[0]));
}

TEST_F(VariableInitializationCheck,
       NonReferenceFromNonReferenceOfDifferentType_ReturnsDifferentTypes)
{
  auto init_expression = init_expression_mock();

  EXPECT_CALL(init_expression, type())
    .WillRepeatedly(ReturnRef(different_type));

  variable_initialization_checker checker;
  const auto init_issues = checker.check(valid_type, init_expression);

  ASSERT_THAT(init_issues.size(), Eq(1));
  EXPECT_TRUE(std::holds_alternative<different_types>(init_issues[0]));
}

TEST_F(VariableInitializationCheck,
       NonReferenceFromReferenceOfDifferentType_ReturnsDifferentTypes)
{
  auto init_expression = init_expression_mock();

  sema_type expression_type{ sema_type_reference{ different_type } };

  EXPECT_CALL(init_expression, type())
    .WillRepeatedly(ReturnRef(expression_type));

  variable_initialization_checker checker;
  const auto init_issues = checker.check(valid_type, init_expression);

  ASSERT_THAT(init_issues.size(), Eq(1));
  EXPECT_TRUE(std::holds_alternative<different_types>(init_issues[0]));
}

TEST_F(VariableInitializationCheck,
       ReferenceFromReferenceOfDifferentType_ReturnsDifferentTypes)
{
  auto init_expression = init_expression_mock();

  sema_type variable_type{ sema_type_reference{ valid_type } };
  sema_type expression_type{ sema_type_reference{ different_type } };

  EXPECT_CALL(init_expression, type())
    .WillRepeatedly(ReturnRef(expression_type));

  variable_initialization_checker checker;
  const auto init_issues = checker.check(variable_type, init_expression);

  ASSERT_THAT(init_issues.size(), Eq(1));
  EXPECT_TRUE(std::holds_alternative<different_types>(init_issues[0]));
}

TEST_F(VariableInitializationCheck,
       ReferenceFromNonReferenceOfDifferentType_ReturnsDifferentTypes)
{
  auto init_expression = init_expression_mock();

  sema_type variable_type{ sema_type_reference{ valid_type } };

  EXPECT_CALL(init_expression, type())
    .WillRepeatedly(ReturnRef(different_type));

  variable_initialization_checker checker;
  const auto init_issues = checker.check(variable_type, init_expression);

  ASSERT_THAT(init_issues.size(), Eq(1));
  EXPECT_TRUE(std::holds_alternative<different_types>(init_issues[0]));
}

TEST_F(
  VariableInitializationCheck,
  ReferenceFromDesignatedInitializers_ReturnsReferenceInitFromTemporaryValue)
{
  auto init_expression = designated_init_expression_mock();

  sema_type variable_type{ sema_type_reference{ valid_type } };

  variable_initialization_checker checker;
  const auto init_issues = checker.check(variable_type, init_expression);

  ASSERT_THAT(init_issues.size(), Eq(1));
  EXPECT_TRUE(std::holds_alternative<reference_init_from_temporary_value>(
    init_issues[0]));
}

TEST_F(VariableInitializationCheck,
       NonReferenceFromDesignatedInitializers_NoMember_ReturnsMemberNotFound)
{
  auto [initializer, _] = member_initializer(token_identifier("member_foo"));

  designated_initializers_node::initializers_t inits;
  inits.emplace_back(std::move(initializer));
  auto init_expression = designated_init_expression_mock(std::move(inits));

  variable_initialization_checker checker;
  const auto init_issues = checker.check(valid_type, init_expression);

  ASSERT_THAT(init_issues.size(), Eq(1));
  EXPECT_TRUE(
    std::holds_alternative<designated_member_not_found>(init_issues[0]));
}

TEST_F(
  VariableInitializationCheck,
  NonReferenceFromDesignatedInitializers_DifferentMemberType_ReturnsDesigDifferentType)
{
  auto [initializer, init_initialization_ptr] =
    member_initializer(token_identifier("member_foo"));

  designated_initializers_node::initializers_t inits;
  inits.emplace_back(std::move(initializer));

  auto init_expression = designated_init_expression_mock(std::move(inits));

  EXPECT_CALL(*init_initialization_ptr, type())
    .WillRepeatedly(ReturnRef(different_type));

  variable_initialization_checker checker;
  const auto init_issues = checker.check(complex_type, init_expression);

  ASSERT_THAT(init_issues.size(), Eq(1));
  EXPECT_TRUE(
    std::holds_alternative<designated_different_types>(init_issues[0]));
}

TEST_F(
  VariableInitializationCheck,
  NonReferenceFromDesignatedInitializers_MultipleMembersInitializers_ReturnsDesigMemberInitializedMultipleTimes)
{
  designated_initializers_node::initializers_t inits;

  const auto member_name = token_identifier("member_foo");
  auto [first_member_init, first_member_init_initialization_ptr] =
    member_initializer(member_name);
  auto [second_member_init, _] = member_initializer(member_name);

  inits.emplace_back(std::move(first_member_init));
  inits.emplace_back(std::move(second_member_init));
  auto init_expression = designated_init_expression_mock(std::move(inits));

  EXPECT_CALL(*first_member_init_initialization_ptr, type())
    .WillRepeatedly(ReturnRef(valid_type));

  variable_initialization_checker checker;
  const auto init_issues = checker.check(complex_type, init_expression);

  ASSERT_THAT(init_issues.size(), Eq(1));
  EXPECT_TRUE(
    std::holds_alternative<designated_member_initialized_multiple_times>(
      init_issues[0]));
}

TEST_F(
  VariableInitializationCheck,
  NonReferenceFromDesignatedInitializers_ReturnsNoIssues)
{
  auto init_expression = designated_init_expression_mock();

  variable_initialization_checker checker;
  const auto init_issues = checker.check(valid_type, init_expression);

  EXPECT_TRUE(init_issues.empty());
}
}
