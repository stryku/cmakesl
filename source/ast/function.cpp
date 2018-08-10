#include "ast/function.hpp"

namespace cmsl
{
    namespace ast
    {
        function::function(type return_type,
                           cmsl::string_view name,
                           std::vector<parameter_declaration> params,
                           std::unique_ptr<ast_node> body)
            : m_return_type{ return_type }
            , m_name{ name }
            , m_params{ std::move(params) }
            , m_body{ std::move(body) }
        {}

        cmsl::string_view function::get_name() const
        {
            return m_name;
        }
    }
}
