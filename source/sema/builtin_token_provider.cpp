#include "sema/builtin_token_provider.hpp"
#include "builtin_token_provider.hpp"

namespace cmsl::sema
{
    builtin_token_provider::builtin_token_provider(std::optional<cmsl::string_view> builtin_documentation_path)
        : m_builtin_documentation_path{ builtin_documentation_path }
    {}

    bool_tokens_provider builtin_token_provider::bool_() const
    {
        return {};
    }
}
