#include "ast/function_node.hpp"
#include "ast/ast_context.hpp"

namespace cmsl
{
    namespace ast
    {
        function::function(ast_context& ctx,
                           type return_type,
                           cmsl::string_view name,
                           std::vector<parameter_declaration> params,
                           std::unique_ptr<block_node> body)
            : ast_node(ast_node_type::function)
            , m_ast_context{ ctx }
            , m_return_type{ return_type }
            , m_name{ name }
            , m_params{ std::move(params) }
            , m_body{ std::move(body) }
        {}

        cmsl::string_view function::get_name() const
        {
            return m_name;
        }

        block_node& function::get_body()
        {
            return *m_body;
        }

        const ast_context& function::get_ast_context() const
        {
            return m_ast_context;
        }
    }
}
