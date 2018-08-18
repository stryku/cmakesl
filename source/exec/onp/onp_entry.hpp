#pragma once

#include "lexer/token/token.hpp"

#include <vector>

namespace cmsl
{
    namespace exec
    {
        namespace onp
        {
            struct onp_entry
            {
                enum class kind
                {
                    oper,
                    constant,
                    identifier
                };

                onp_entry(kind k, lexer::token::token_container_t tokens)
                        : k{ k }
                        , tokens{ tokens }
                {}

                onp_entry(kind k, lexer::token::token token)
                        : k{ k }
                        , tokens{ token }
                {}

                kind k;
                lexer::token::token_container_t tokens; //todo consider using kind of small vector
            };
        }
    }
}