#include "ast/type.hpp"
#include "ast/type_kind.hpp"
#include "common/algorithm.hpp"

namespace cmsl
{
    namespace ast
    {
        type::type(lexer::token::token token)
            : m_name{ token.str() }
            , m_kind{ type_kind_from_string(m_name) }
        {}

        type::type(cmsl::string_view name, type_kind kind)
            : m_name{ name }
            , m_kind{ kind }
        {}

        bool type::is_builtin() const
        {
            return m_kind != type_kind::k_user;
        }

        cmsl::string_view type::get_name() const
        {
            return m_name;
        }
    }
}
