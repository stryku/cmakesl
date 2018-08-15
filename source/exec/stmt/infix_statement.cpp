#include "exec/stmt/infix_statement.hpp"
#include "exec/exec.hpp"
#include "ast/infix_node.hpp"
#include "exec/instance.hpp"
#include "ast/type_kind.hpp"
#include "ast/ast_context.hpp"
#include "exec/named_instance.hpp"
#include "exec/stmt/infix_to_onp.hpp"
#include "exec/stmt/onp_executor.hpp"

#include "lexer/token/token.hpp"

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
                onp_executor{ onp_tokens, e, m_result }.execute();
            }

            infix_statement::tokens_container_t infix_statement::to_onp(executor& e) const
            {
                const auto& tokens = m_node.get_tokens();
                return infix_to_onp{ tokens, e.get_ast_ctx() }.convert();
            }
        }
    }
}
