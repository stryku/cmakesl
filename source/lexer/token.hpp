#pragma once

#include "common/source_view.hpp"
#include "common/source_location.hpp"
#include "common/string.hpp"
#include "token_type.hpp"

#include <vector>

namespace cmsl::lexer
{
    class token
    {
    public:
        using token_type_t = cmsl::lexer::token_type;

        // Creates token with invalid begin and end locations
        explicit token();
        explicit token(token_type_t type);
        explicit token(token_type_t type, const source_range& src_range, cmsl::source_view source);

        token(const token&) = default;
        token& operator=(const token&) = default;
        token(token&&) = default;
        token& operator=(token&&) = default;

        bool is_valid() const;
        token_type_t get_type() const;

        static token undef();

        cmsl::string_view str() const;

        source_range src_range() const;
        cmsl::source_view source() const;

        bool operator==(const token& rhs) const;

        friend std::ostream& operator<<(std::ostream& out, const token& t);

    private:
        token_type_t m_type;
        source_range m_source_range;
        cmsl::source_view m_source;
    };

    using token_container_t = std::vector<token>;

    template<unsigned N>
    token make_token(lexer::token_type token_type, const char (&tok)[N])
    {
        // N counts also '\0'
        const auto src_range = source_range{
                source_location{ 1u, 1u, 0u },
                source_location{ 1u, N, N - 1u }
        };
        return token{ token_type, src_range, cmsl::source_view{ tok } };
    }
}


namespace std
{
    template <>
    struct hash<cmsl::lexer::token>
    {
        std::size_t operator()(const cmsl::lexer::token& token) const
        {
            std::size_t h = std::hash<std::string_view>{}(token.str());

            const auto range = token.src_range();

            h ^= range.begin.line;
            h ^= range.begin.column;
            h ^= range.begin.absolute;

            h ^= range.end.line;
            h ^= range.end.column;
            h ^= range.end.absolute;

            return h;
        }

    };
}
