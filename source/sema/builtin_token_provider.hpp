#pragma once

#include "lexer/token.hpp"

#include <optional>

namespace cmsl::sema
{
    struct bool_tokens_provider;

    class builtin_token_provider
    {
    public:
        explicit builtin_token_provider(std::optional<cmsl::string_view> builtin_documentation_path);

        bool_tokens_provider bool_() const;

    private:
        std::optional<cmsl::string_view> m_builtin_documentation_path;
    };
}
