#include "exec/instance/simple_unnamed_instance.hpp"
#include "exec/infix/class_member_access_expression.cpp"
#include "exec/infix/id_expression.hpp"

#include "test/exec/mock/infix_expression_mock.hpp"
#include "test/exec/mock/instance_mock.hpp"
#include "test/exec/eval/ExpressionEvaluationTest.hpp"
#include "test/common/tokens.hpp"

namespace cmsl
{
    namespace test
    {
        namespace exec
        {
            namespace infix
            {
                namespace class_member_access_expression
                {
                    using testing::Return;
                    using testing::NotNull;
                    using testing::_;

                    using namespace cmsl::test::common;

                    using ClassMemberAccess = ExpressionEvaluationTest<testing::Test>;

                    TEST_F(ClassMemberAccess, GetCorrectMember)
                    {
                        const auto& int_type = *m_ctx_provider.get_ast_ctx().find_type("int");
                        auto member_instance = std::make_unique<cmsl::exec::inst::simple_unnamed_instance>(int_type);
                        auto class_instance = std::make_unique<instance_mock>();
                        auto infix_expr = std::make_unique<infix_expression_mock>();

                        EXPECT_CALL(*infix_expr, evaluate(_))
                                    .WillOnce(Return(class_instance.get()));

                        EXPECT_CALL(*class_instance, get_member(_))
                                    .WillOnce(Return(member_instance.get()));

                        auto class_member_access_expr = std::make_unique<cmsl::exec::infix::class_member_access_expression>(std::move(infix_expr), token_identifier());
                        auto result = class_member_access_expr->evaluate(m_eval_ctx);

                        EXPECT_THAT(result, member_instance.get());
                    }
                }
            }
        }
    }
}
