#pragma once

#include <string>
#include <vector>

namespace cmsl
{
    namespace lexer
    {
        namespace token
        {
            class token;
        }

        class lexer
        {
        public:
            using tokens_container_t = std::vector<token::token>;

            tokens_container_t lex(const std::string);

        private:
            token::token get_token(const std::string& source) const;
        };
    }
}
