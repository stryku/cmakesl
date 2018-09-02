#include "exec/stmt/infix_statement.hpp"
#include "exec/execution.hpp"
#include "ast/infix_node.hpp"
#include "ast/type_kind.hpp"
#include "ast/ast_context.hpp"
#include "exec/onp/infix_to_onp.hpp"
#include "exec/onp/onp_executor.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace stmt
        {
            infix_statement::infix_statement(const ast::infix_node& node, inst::instance_value_t& result)
                : m_node{ node }
                , m_result{ result }
            {}

            void infix_statement::execute(execution& e)
            {
                const auto onp_tokens = to_onp(e);
                onp::onp_executor{ onp_tokens, e, m_result }.execute();
            }

            infix_statement::tokens_container_t infix_statement::to_onp(execution& e) const
            {
                const auto& tokens = m_node.get_tokens();
                return onp::infix_to_onp{ tokens, e.get_ast_ctx() }.convert();
            }
        }
    }
}
