#pragma once

#include "common/source_location.hpp"
#include "common/string.hpp"
#include "lexer/token/token_type.hpp"

#include <vector>

namespace cmsl
{
    namespace lexer
    {
        namespace token
        {
            class token
            {
            public:
                using token_type_t = cmsl::lexer::token::token_type;

                // Creates token with invalid begin and end locations
                explicit token(token_type_t type);
                explicit token(token_type_t type, const source_range& src_range, cmsl::string_view source);

                token(const token&) = default;
                token& operator=(const token&) = default;
                token(token&&) = default;
                token& operator=(token&&) = default;

                bool is_valid() const;
                token_type_t get_type() const;

                static token undef();

                cmsl::string_view str() const;

                bool operator==(const token& rhs) const;

                friend std::ostream& operator<<(std::ostream& out, const token& t);

            private:
                token_type_t m_type;
                source_range m_source_range;
                cmsl::string_view m_source;
            };

            using token_container_t = std::vector<token>;
        }
    }
}
