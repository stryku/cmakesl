#include "ast/user_function_node.hpp"
#include "ast/ast_context.hpp"

namespace cmsl
{
    namespace ast
    {
        user_function_node::user_function_node(ast_context& ctx,
                                     const type& return_type,
                                     cmsl::string_view name,
                                     std::vector<parameter_declaration> params,
                                     std::unique_ptr<block_node> body)
            : m_ast_context{ ctx }
            , m_return_type{ return_type }
            , m_name{ name }
            , m_params{ std::move(params) }
            , m_body{ std::move(body) }
        {}

        cmsl::string_view user_function_node::get_name() const
        {
            return m_name;
        }

        const block_node& user_function_node::get_body() const
        {
            return *m_body;
        }

        const ast_context& user_function_node::get_ast_context() const
        {
            return m_ast_context;
        }

        const std::vector<parameter_declaration>& user_function_node::get_params_declarations() const
        {
            return m_params;
        }

        const type &user_function_node::get_return_type() const
        {
            return m_return_type;
        }

        const type &user_function_node::get_type() const
        {
            return m_return_type;
        }
    }
}
