#include "ast/list_type.hpp"

#include "ast/ast_context.hpp"
#include "ast/fundamental_function.hpp"

namespace cmsl
{
    namespace ast
    {
        list_type::list_type(std::string name, const ast_context* ast_ctx, const type& value_type)
            : type{ name.c_str(), type_kind::k_list, ast_ctx }
            , m_value_type{ value_type }
        {}
    }
}

