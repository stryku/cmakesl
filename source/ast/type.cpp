#include "ast/type.hpp"
#include "ast/type_kind.hpp"
#include "common/algorithm.hpp"
#include "ast/ast_context.hpp"

namespace cmsl
{
    namespace ast
    {
        type::type(lexer::token::token token)
            : m_name{ token.str() }
            , m_kind{ type_kind_from_string(m_name) }
            , m_ast_ctx{ nullptr }
        {}

        type::type(cmsl::string_view name, type_kind kind, const ast_context* ast_ctx)
            : m_name{ name }
            , m_kind{ kind }
            , m_ast_ctx{ ast_ctx }
        {}

        bool type::is_fundamental() const
        {
            return m_kind != type_kind::k_user;
        }

        std::string type::get_name() const
        {
            return m_name.to_string();
        }

        bool type::has_function(cmsl::string_view name) const
        {
            return get_function(name) != nullptr;
        }

        const function* type::get_function(cmsl::string_view name) const
        {
            return m_ast_ctx->find_function(name);
        }

        type_kind type::get_kind() const
        {
            return m_kind;
        }

        bool type::operator==(const type &other) const
        {
            // For every type, there only one instance during runtime. Pointers comparison is enough
            return this == &other;
        }

        bool type::operator!=(const type &other) const
        {
            return !(*this == other);
        }
    }
}
