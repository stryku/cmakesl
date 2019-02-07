#include "exec/expression_evaluation_visitor.hpp"

#include "sema/sema_context.hpp"
#include "sema/sema_type.hpp"

#include "test/common/tokens.hpp"
#include "test/exec/mock/function_caller_mock.hpp"
#include "test/exec/mock/identifiers_context_mock.hpp"
#include "test/exec/mock/instances_holder_mock.hpp"
#include "test/exec/mock/instance_mock.hpp"

#include <gmock/gmock.h>
#include "exec/instance/int_alias.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace test
        {
            using ::testing::_;
            using ::testing::ByRef;
            using ::testing::Return;
            using ::testing::Eq;
            using ::testing::StrictMock;

            using namespace cmsl::test::common;

            const sema::sema_context valid_context;
            const sema::sema_type valid_type{ valid_context, token_identifier("foo"), {} };

            class ExpressionEvaluationVisitorTest : public ::testing::Test
            {
            protected:
                StrictMock<inst::test::instance_mock> m_instance_mock;
                StrictMock<identifiers_context_mock> m_ids_ctx;
                StrictMock<function_caller2_mock> m_caller;
                StrictMock<inst::test::instances_holder_mock> m_instances;
                expression_evaluation_context m_ctx{
                        m_caller,
                        m_instances,
                        m_ids_ctx
                };
            };

            TEST_F(ExpressionEvaluationVisitorTest, Visit_BoolValue_CreatesInstanceAndStoresAsResult)
            {
                expression_evaluation_visitor visitor{ m_ctx };

                // Todo: consider testing false too.
                sema::bool_value_node true_node{valid_type, true};
                inst::instance_value_t value{true};

                EXPECT_CALL(m_instances, create2(_))
                        .WillOnce(Return(&m_instance_mock));

                visitor.visit(true_node);

                EXPECT_THAT(visitor.result, Eq(&m_instance_mock));
            }

            TEST_F(ExpressionEvaluationVisitorTest, Visit_IntValue_CreatesInstanceAndStoresAsResult)
            {
                expression_evaluation_visitor visitor{ m_ctx };

                // Todo: use int alias instead of std::int64_t
                const auto test_value = std::int64_t{42};
                sema::int_value_node node{valid_type, test_value };
                inst::instance_value_t value{test_value};

                EXPECT_CALL(m_instances, create2(_))
                        .WillOnce(Return(&m_instance_mock));

                visitor.visit(node);

                EXPECT_THAT(visitor.result, Eq(&m_instance_mock));
            }

            TEST_F(ExpressionEvaluationVisitorTest, Visit_DoubleValue_CreatesInstanceAndStoresAsResult)
            {
                expression_evaluation_visitor visitor{ m_ctx };

                const auto test_value = 42.42;
                sema::double_value_node node{valid_type, test_value };
                inst::instance_value_t value{test_value};

                EXPECT_CALL(m_instances, create2(_))
                        .WillOnce(Return(&m_instance_mock));

                visitor.visit(node);

                EXPECT_THAT(visitor.result, Eq(&m_instance_mock));
            }

            TEST_F(ExpressionEvaluationVisitorTest, Visit_StringValue_CreatesInstanceAndStoresAsResult)
            {
                expression_evaluation_visitor visitor{ m_ctx };

                const auto test_value = std::string{"42"};
                sema::string_value_node node{valid_type, test_value };
                inst::instance_value_t value{test_value};

                EXPECT_CALL(m_instances, create2(_))
                        .WillOnce(Return(&m_instance_mock));

                visitor.visit(node);

                EXPECT_THAT(visitor.result, Eq(&m_instance_mock));
            }

            TEST_F(ExpressionEvaluationVisitorTest, Visit_Identifier_CreatesInstanceAndStoresAsResult)
            {
                expression_evaluation_visitor visitor{ m_ctx };

                const auto id_token = token_identifier("foo");
                sema::id_node node{valid_type, id_token };

                EXPECT_CALL(m_ids_ctx, lookup_identifier(id_token.str()))
                        .WillOnce(Return(&m_instance_mock));

                visitor.visit(node);

                EXPECT_THAT(visitor.result, Eq(&m_instance_mock));
            }
        }
    }
}
