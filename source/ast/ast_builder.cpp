#include "ast/ast_builder.hpp"
#include "ast/builtin_ast_context.hpp"
//#include "ast/expr/block_node.hpp"
//#include "ast/expr/constant_expression.hpp"
//#include "ast/expr/infix_node.hpp"
#include "lexer/token/token.hpp"

//#include "ast/binary_operator.hpp"
//#include "ast/integral_constant.hpp"
#include "ast/type.hpp"
#include "ast/function_node.hpp"

#include <stack>

namespace cmsl
{
    namespace ast
    {
        ast_builder::ast_builder()
        {
            auto builtin_ctx = std::make_unique<builtin_ast_context>();
            m_current_ast_ctx = builtin_ctx.get();
            m_contextes.emplace_back(std::move(builtin_ctx));
        }

        std::unique_ptr<ast_context> ast_builder::build(ast_context& builtin_ast_ctx,
                                                        errors::errors_observer& err_observer,
                                                        const tokens_container_t& tokens)
        {
            auto ctx = std::make_unique<ast_context>(&builtin_ast_ctx);
            m_parser = std::make_unique<parser>(err_observer, tokens);


            while(!m_parser->is_at_end())
            {
                auto fun = m_parser->get_function(*ctx);
                ctx->add_function(std::move(fun));
            }

            return std::move(ctx);
        }
    }
}
