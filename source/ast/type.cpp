#include "ast/type.hpp"

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

            return std::any_of(std::cbegin(builtin_types), std::cend(builtin_types),
                               [token_type = m_token.get_type()](const auto builtin_type)
                               {
                                   return token_type == builtin_type;
                               });
        }
    }
}
