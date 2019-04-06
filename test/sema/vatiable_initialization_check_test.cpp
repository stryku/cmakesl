
#include "sema/sema_nodes.hpp"
#include "sema/sema_context.hpp"
#include "sema/variable_initialization_checker.hpp"

#include "test/common/tokens.hpp"
#include "test/sema/mock/expression_node_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test
{
    using ::testing::StrictMock;
    using ::testing::Return;
    using ::testing::ReturnRef;
    using ::testing::IsNull;
    using ::testing::NotNull;
    using ::testing::Eq;
    using ::testing::_;

    using namespace cmsl::test::common;

    const sema_context valid_context;
    const sema_type valid_type{ valid_context, ast::type_representation{ token_identifier("foo") }, {} };
    const sema_type different_type{ valid_context, ast::type_representation{ token_identifier("bar") }, {} };

    using check_result_t = variable_initialization_checker::check_result;

    TEST(VariableInitializationCheck, NonReferenceInitializedFromNonReference_ReturnsCanInitialize)
    {
        StrictMock<expression_node_mock> init_expression;

        EXPECT_CALL(init_expression, type())
                .WillOnce(ReturnRef(valid_type));

        variable_initialization_checker checker;
        const auto result = checker.check(valid_type, init_expression);

        EXPECT_THAT(result, Eq(check_result_t::can_init));
    }

    TEST(VariableInitializationCheck, NonReferenceInitializedFromReference_ReturnsCanInitialize)
    {
        StrictMock<expression_node_mock> init_expression;

        sema_type expression_type{ sema_type_reference{ valid_type } };

        EXPECT_CALL(init_expression, type())
                .WillRepeatedly(ReturnRef(expression_type));

        variable_initialization_checker checker;
        const auto result = checker.check(valid_type, init_expression);

        EXPECT_THAT(result, Eq(check_result_t::can_init));
    }

    TEST(VariableInitializationCheck, ReferenceInitializedFromReference_ReturnsCanInitialize)
    {
        StrictMock<expression_node_mock> init_expression;

        sema_type variable_type{ sema_type_reference{ valid_type } };
        sema_type expression_type{ sema_type_reference{ valid_type } };

        EXPECT_CALL(init_expression, type())
                .WillRepeatedly(ReturnRef(expression_type));

        variable_initialization_checker checker;
        const auto result = checker.check(variable_type, init_expression);

        EXPECT_THAT(result, Eq(check_result_t::can_init));
    }

    TEST(VariableInitializationCheck, ReferenceInitializedFromNonReferenceNonTemporaryValue_ReturnsCanInitialize)
    {
        StrictMock<expression_node_mock> init_expression;

        sema_type variable_type{ sema_type_reference{ valid_type } };

        EXPECT_CALL(init_expression, type())
                .WillRepeatedly(ReturnRef(valid_type));

        EXPECT_CALL(init_expression, produces_temporary_value())
                .WillRepeatedly(Return(false));

        variable_initialization_checker checker;
        const auto result = checker.check(variable_type, init_expression);

        EXPECT_THAT(result, Eq(check_result_t::can_init));
    }

    TEST(VariableInitializationCheck, ReferenceInitializedFromNonReferenceTemporaryValue_ReturnsReferenceInitFromTemporaryValue)
    {
        StrictMock<expression_node_mock> init_expression;

        sema_type variable_type{ sema_type_reference{ valid_type } };

        EXPECT_CALL(init_expression, type())
                .WillRepeatedly(ReturnRef(valid_type));

        EXPECT_CALL(init_expression, produces_temporary_value())
                .WillRepeatedly(Return(true));

        variable_initialization_checker checker;
        const auto result = checker.check(variable_type, init_expression);

        EXPECT_THAT(result, Eq(check_result_t::reference_init_from_temporary_value));
    }

    TEST(VariableInitializationCheck, NonReferenceFromNonReferenceOfDifferentType_ReturnsDifferentTypes)
    {
        StrictMock<expression_node_mock> init_expression;

        EXPECT_CALL(init_expression, type())
                .WillRepeatedly(ReturnRef(different_type));

        variable_initialization_checker checker;
        const auto result = checker.check(valid_type, init_expression);

        EXPECT_THAT(result, Eq(check_result_t::different_types));
    }

    TEST(VariableInitializationCheck, NonReferenceFromReferenceOfDifferentType_ReturnsDifferentTypes)
    {
        StrictMock<expression_node_mock> init_expression;

        sema_type expression_type{ sema_type_reference{ different_type } };

        EXPECT_CALL(init_expression, type())
                .WillRepeatedly(ReturnRef(expression_type));

        variable_initialization_checker checker;
        const auto result = checker.check(valid_type, init_expression);

        EXPECT_THAT(result, Eq(check_result_t::different_types));
    }

    TEST(VariableInitializationCheck, ReferenceFromReferenceOfDifferentType_ReturnsDifferentTypes)
    {
        StrictMock<expression_node_mock> init_expression;

        sema_type variable_type{ sema_type_reference{ valid_type } };
        sema_type expression_type{ sema_type_reference{ different_type } };

        EXPECT_CALL(init_expression, type())
                .WillRepeatedly(ReturnRef(expression_type));

        variable_initialization_checker checker;
        const auto result = checker.check(variable_type, init_expression);

        EXPECT_THAT(result, Eq(check_result_t::different_types));
    }

    TEST(VariableInitializationCheck, ReferenceFromNonReferenceOfDifferentType_ReturnsDifferentTypes)
    {
        StrictMock<expression_node_mock> init_expression;

        sema_type variable_type{ sema_type_reference{ valid_type } };

        EXPECT_CALL(init_expression, type())
                .WillRepeatedly(ReturnRef(different_type));

        variable_initialization_checker checker;
        const auto result = checker.check(variable_type, init_expression);

        EXPECT_THAT(result, Eq(check_result_t::different_types));
    }
}
