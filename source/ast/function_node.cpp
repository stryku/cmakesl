#include "ast/function_node.hpp"

namespace cmsl
{
    namespace ast
    {
        function::function(type return_type,
                           cmsl::string_view name,
                           std::vector<parameter_declaration> params,
                           std::unique_ptr<block_node> body)
            : ast_node(ast_node_type::function)
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
    }
}
