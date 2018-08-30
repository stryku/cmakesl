#include "exec/stmt/while_statement.hpp"

#include "ast/conditional_node.hpp"
#include "ast/while_node.hpp"

#include "exec/exec.hpp"
#include "exec/stmt/infix_statement.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace stmt
        {
            while_statement::while_statement(const ast::while_node &node)
                : m_node{ node }
            {}

            void while_statement::execute(executor &e)
            {
                while(condition_passed(e))
                {
                    e.block(m_node.get_node().get_block());
                }
            }

            bool while_statement::condition_passed(executor &e)
            {
                const auto& condition = m_node.get_node().get_condition();
                inst::instance_value_t infix_result{};
                auto infix = infix_statement{ condition, infix_result };
                infix.execute(e);

                // todo convert result to bool
                return boost::get<int>(infix_result) != 0;
            }
        }
    }
}
