#include "exec/stmt/while_statement.hpp"

#include "ast/conditional_node.hpp"
#include "ast/while_node.hpp"

#include "exec/source_executor.hpp"
#include "exec/stmt/infix_statement.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/instance/instance_converter.hpp"
#include "ast/ast_context.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace stmt
        {
            while_statement::while_statement(const ast::while_node &node)
                : m_node{ node }
            {}

            void while_statement::execute(source_executor &e)
            {
                while(condition_passed(e))
                {
                    e.block(m_node.get_node().get_block());
                }
            }

            bool while_statement::condition_passed(source_executor &e)
            {
                const auto& condition = m_node.get_node().get_condition();
                inst::instance_value_t infix_result{};
                auto infix = infix_statement{ condition, infix_result };
                infix.execute(e);

                // Convert result to bool
                inst::instances_holder instances{ e };
                inst::instance_converter converter{ instances };
                auto result_instance = instances.create(infix_result);
                const auto bool_type = e.get_ast_ctx().find_type("bool");
                auto bool_instance = converter.convert_to_type(result_instance, *bool_type);
                return boost::get<bool>(bool_instance->get_value()) == true;
            }
        }
    }
}
