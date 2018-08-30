#include "exec/stmt/if_else_statement.hpp"

#include "ast/if_else_node.hpp"
#include "ast/conditional_node.hpp"
#include "exec/stmt/infix_statement.hpp"
#include "exec/exec.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace stmt
        {
            if_else_statement::if_else_statement(const ast::if_else_node &node)
                : m_node{ node }
            {}

            void if_else_statement::execute(executor &e)
            {
                for(const auto& cond_node : m_node.get_ifs())
                {
                    const auto& condition = cond_node->get_condition();
                    inst::instance_value_t infix_result{};
                    auto infix = infix_statement{ condition, infix_result };
                    infix.execute(e);

                    // todo convert result to bool
                    if(boost::get<int>(infix_result) != 0)
                    {
                        e.block(cond_node->get_block());
                        return;
                    }
                }

                // At this point we know that none of the ifs condition was true

                const auto else_node = m_node.get_else();
                if(else_node)
                {
                    e.block(*else_node);
                }
            }
        }
    }
}
