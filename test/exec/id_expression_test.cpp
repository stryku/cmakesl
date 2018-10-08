#include "test/exec/eval/ExpressionEvaluationTest.hpp"

#include "exec/infix/id_expression.hpp"
#include "exec/instance/simple_unnamed_instance.hpp"

#include "test/common/tokens.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            namespace test
            {
                using testing::Eq;
                using testing::NotNull;
                using testing::DoubleEq;

                using namespace cmsl::test::common;

                using IdExpression = ExpressionEvaluationTest<testing::Test>;

                TEST_F(IdExpression, FindCorrectVariable)
                {
                    using inst_t = inst::simple_unnamed_instance;
                    const auto name = "name";
                    const auto value = 3.4;
                    auto instance = std::make_unique<inst_t>(*m_ctx_provider.get_ast_ctx()
                                                                            .find_type("int"),
                                                             value);
                    auto instance_ptr = instance.get();

                    m_ctx_provider.get_exec_ctx()
                                  .enter_scope();
                    m_ctx_provider.get_exec_ctx()
                                  .add_variable(name, std::move(instance));
                    auto expr = std::make_unique<id_expression>(token_identifier(name));
                    auto result = expr->evaluate(m_eval_ctx);
                    EXPECT_THAT(result, Eq(instance_ptr));
                    EXPECT_THAT(result_val<double>(result), DoubleEq(value));
                }
            }
        }
    }
}
