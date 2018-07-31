#include "ast/type.hpp"
#include "common/algorithm.hpp"

namespace cmsl
{
    namespace ast
    {
        type::type(lexer::token::token token)
            : m_token{ token }
        {}

        bool type::is_builtin() const
        {
            const auto builtin_types = {
                token_type_t::t_int,
                token_type_t::t_real
            };

            return cmsl::contains(builtin_types, m_token.get_type());
        }

        cmsl::string_view type::get_name() const
        {
            return m_token.str();
        }
    }
}
