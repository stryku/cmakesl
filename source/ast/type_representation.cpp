#include "ast/type_representation.hpp"


namespace cmsl::ast
{
        type_representation::type_representation(lexer::token::token primary_name,
                                     lexer::token::token reference_token,
                                     std::vector<type_representation> nested_types)
                : m_tokens{ { primary_name } }
                , m_nested_types{ std::move(nested_types) }
                , m_reference_token{ reference_token }
        {}
        type_representation::type_representation(lexer::token::token_container_t tokens,
                                     lexer::token::token reference_token,
                                     std::vector<type_representation> nested_types)
                : m_tokens{ std::move(tokens) }
                , m_nested_types{ std::move(nested_types) }
                , m_reference_token{ reference_token }
        {}

        type_representation::type_representation(lexer::token::token primary_name,
                                     is_reference_tag,
                                     std::vector<type_representation> nested_types)
                : m_tokens{ { primary_name } }
                , m_nested_types{ std::move(nested_types) }
                , m_is_reference{ true }
        {}
        type_representation::type_representation(lexer::token::token_container_t tokens,
                                     is_reference_tag,
                                     std::vector<type_representation> nested_types)
                : m_tokens{ std::move(tokens) }
                , m_nested_types{ std::move(nested_types) }
                , m_is_reference{ true }
        {}
        type_representation::type_representation(lexer::token::token primary_name,
                                     std::vector<type_representation> nested_types)
                : m_tokens{ { primary_name } }
                , m_nested_types{ std::move(nested_types) }
        {}
        type_representation::type_representation(lexer::token::token_container_t tokens,
                                     std::vector<type_representation> nested_types)
                : m_tokens{ std::move(tokens) }
                , m_nested_types{ std::move(nested_types) }
        {}

        lexer::token::token type_representation::primary_name() const
        {
            return m_tokens.front();
        }

        const std::vector<type_representation>& type_representation::nested_types() const
        {
            return m_nested_types;
        }

        const lexer::token::token_container_t& type_representation::tokens() const
        {
            return m_tokens;
        }

        std::string type_representation::to_string() const
        {
            auto str = to_string_without_reference();
            if(is_reference())
            {
                str += '&';
            }

            return str;
        }

        std::string type_representation::to_string_without_reference() const
        {
            // Todo: create string once, store it somewhere (some strings container) and return string_view.
            std::string result;

            for(const auto& token : m_tokens)
            {
                result += token.str();
            }

            return result;
        }

        bool type_representation::operator==(const type_representation& rhs) const
        {
            // For now, we need to compare strings because when we execute add_subdirectory, a new builtin context is created there.
            // Because of that, we have two instances representing e.g. int type. That should be fixed at some point.
            return this == &rhs || to_string() == rhs.to_string();
        }

        bool type_representation::operator!=(const type_representation& rhs) const
        {
            return !(*this == rhs);
        }

        bool type_representation::is_generic() const
        {
            return !m_nested_types.empty();
        }

        bool type_representation::is_reference() const
        {
            return m_reference_token.has_value() || m_is_reference;
        }

}