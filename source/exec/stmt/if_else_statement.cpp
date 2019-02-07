#include "exec/stmt/if_else_statement.hpp"

#include "ast/if_else_node.hpp"
#include "ast/conditional_node.hpp"
#include "exec/stmt/infix_statement.hpp"
#include "exec/execution.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/instance/instance_converter.hpp"
#include "ast/ast_context.hpp"
#include "ast/infix_node.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace stmt
        {
            if_else_statement::if_else_statement(const ast::if_else_node &node)
                : m_node{ node }
            {}

            void if_else_statement::execute(execution &e)
            {
                for(const auto& cond_node : m_node.get_ifs())
                {
                    const auto& condition = cond_node->get_condition();
                    std::unique_ptr<inst::instance> infix_result;
                    auto infix = infix_statement{ dynamic_cast<const ast::infix_node&>(condition), infix_result };// Todo: get rid of dynamic cast
                    infix.execute(e);

                    // Convert result to bool
                    inst::instances_holder instances{ e };
                    inst::instance_converter converter{ instances };
                    auto result_instance = instances.create(infix_result->get_value());
                    const auto bool_type = e.get_ast_ctx().find_type("bool");
                    auto bool_instance = converter.convert_to_type(result_instance, *bool_type);

                    if(bool_instance->get_value().get_bool())
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
