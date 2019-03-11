#pragma once

#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        // A structure that holds tokens that represent given type. E.g.:
        // double => tokens vector is: {double}
        // map<int, list<string>> => tokens vector is: {map, int, list, string}
        // Todo: move to sema namespace
        struct type_name_reference
        {
            type_name_reference() = default;
            type_name_reference(lexer::token::token token)
                : tokens{ 1, token }
                , end_token{ token }
            {}
            type_name_reference(std::vector<lexer::token::token> tokens, lexer::token::token end_token)
                : tokens{ std::move(tokens) }
                , end_token{ end_token }
            {}
            //lexer::token::token begin;
            //lexer::token::token end;
            std::vector<lexer::token::token> tokens;

            // We need to keep it to easily format errors in case of generic types, which ends with '>'.
            lexer::token::token end_token;

            cmsl::string_view to_string() const
            {
                const auto b = tokens.front().str().data();
                return cmsl::string_view{ b, static_cast<size_t>(end_token.str().cend() - b)};
            }

            bool operator==(const type_name_reference& rhs) const
            {
                return tokens == rhs.tokens;
            }

            bool operator!=(const type_name_reference& rhs) const
            {
                return !(*this == rhs);
            }
        };
    }
}
