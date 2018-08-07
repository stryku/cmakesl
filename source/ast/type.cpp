#include "ast/type.hpp"
#include "common/algorithm.hpp"

namespace cmsl
{
    namespace ast
    {
        type::type(lexer::token::token token)
            : m_name{ token.str() }
        {}

        type::type(type_kind kind)
            : m_name{}
            , m_kind{ kind }
        {}

        bool type::is_builtin() const
        {
            const auto builtin_types = {
                "int",
                "real"
            };

            return cmsl::contains(builtin_types, m_name);
        }

        cmsl::string_view type::get_name() const
        {
            return m_name;
        }
    }
}
