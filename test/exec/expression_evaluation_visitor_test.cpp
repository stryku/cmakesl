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

            TEST(ExpressionEvaluationVisitorTest, Visit_BoolValue_CreatesInstanceAndStoresAsResult)
            {
                StrictMock<inst::test::instance_mock> instance_mock;
                StrictMock<identifiers_context_mock> ids_ctx;
                StrictMock<function_caller2_mock> caller;
                StrictMock<inst::test::instances_holder_mock> instances;
                expression_evaluation_context ctx{
                        caller,
                        instances,
                        ids_ctx
                };

                expression_evaluation_visitor visitor{ ctx };

                // Todo: consider testing false too.
                sema::bool_value_node true_node{valid_type, true};
                inst::instance_value_t value{true};

                EXPECT_CALL(instances, create2(_))
                    .WillOnce(Return(&instance_mock));

                visitor.visit(true_node);

                EXPECT_THAT(visitor.result, Eq(&instance_mock));
            }
        }
    }
}
