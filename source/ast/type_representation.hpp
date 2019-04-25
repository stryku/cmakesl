#pragma once

#include "lexer/token/token.hpp"

namespace cmsl::ast
{
        class type_representation
        {
        public:
            struct is_reference_tag{};

            explicit type_representation(lexer::token::token primary_name,
                                         lexer::token::token reference_token,
                                         std::vector<type_representation> nested_types  = {});
            explicit type_representation(lexer::token::token_container_t tokens,
                                         lexer::token::token reference_token,
                                         std::vector<type_representation> nested_types  = {});

            explicit type_representation(lexer::token::token primary_name,
                                         is_reference_tag,
                                         std::vector<type_representation> nested_types  = {});
            explicit type_representation(lexer::token::token_container_t tokens,
                                         is_reference_tag,
                                         std::vector<type_representation> nested_types  = {});
            explicit type_representation(lexer::token::token primary_name,
                                         std::vector<type_representation> nested_types  = {});
            explicit type_representation(lexer::token::token_container_t tokens,
                                         std::vector<type_representation> nested_types  = {});

            lexer::token::token primary_name() const;
            const std::vector<type_representation>& nested_types() const;
            const lexer::token::token_container_t& tokens() const;

            std::string to_string() const;
            std::string to_string_without_reference() const;

            bool operator==(const type_representation& rhs) const;
            bool operator!=(const type_representation& rhs) const;

            bool is_generic() const;
            bool is_reference() const;

        private:
            lexer::token::token_container_t m_tokens;
            std::optional<lexer::token::token> m_reference_token;
            bool m_is_reference{ false };

            // 'template parameters' of generic types.
            std::vector<type_representation> m_nested_types;
        };
}
