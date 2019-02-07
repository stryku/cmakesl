#include "exec/stmt/while_statement.hpp"

#include "ast/conditional_node.hpp"
#include "ast/while_node.hpp"

#include "exec/execution.hpp"
#include "exec/stmt/infix_statement.hpp"
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
            while_statement::while_statement(const ast::while_node &node)
                : m_node{ node }
            {}

            void while_statement::execute(execution &e)
            {
                while(condition_passed(e))
                {
                    e.block(m_node.get_node().get_block());
                }
            }

            bool while_statement::condition_passed(execution &e)
            {
                const auto& condition = m_node.get_node().get_condition();
                std::unique_ptr<inst::instance> infix_result;
                auto infix = infix_statement{dynamic_cast<const ast::infix_node&>(condition), infix_result }; // Todo: get rid of dynamic cast
                infix.execute(e);

                // Convert result to bool
                inst::instances_holder instances{ e };
                inst::instance_converter converter{ instances };
                const auto bool_type = e.get_ast_ctx().find_type("bool");
                auto bool_instance = converter.convert_to_type(infix_result.get(), *bool_type);
                return bool_instance->get_value().get_bool();
            }
        }
    }
}
