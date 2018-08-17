#include "exec/stmt/infix_statement.hpp"
#include "exec/exec.hpp"
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
            infix_statement::infix_statement(const ast::infix_node& node, int& result)
                : m_node{ node }
                , m_result{ result }
            {}

            void infix_statement::execute(executor& e)
            {
                const auto onp_tokens = to_onp(e);
                onp::onp_executor{ onp_tokens, e, m_result }.execute();
            }

            std::vector<onp::onp_entry> infix_statement::to_onp(executor& e) const
            {
                const auto& tokens = m_node.get_tokens();
                return onp::infix_to_onp{ tokens, e.get_ast_ctx() }.convert();
            }
        }
    }
}
