#pragma once

#include "ast/user_function_node.hpp"
#include "lexer/token/token.hpp"
#include "type_kind.hpp"

namespace cmsl
{
    namespace ast
    {
        enum class type_kind;
        class ast_context;

        class type
        {
        public:
            using token_type_t = lexer::token::token_type;

            struct member_info
            {
                lexer::token::token name;
                const type& ty;
            };

            explicit type(lexer::token::token token);
            explicit type(cmsl::string_view name, type_kind kind, const ast_context* ast_ctx, std::vector<member_info> members = {});

            type(const type&) = delete;
            type& operator=(const type&) = delete;

            virtual ~type() = default;

            bool is_fundamental() const;
            bool is_generic() const;
            bool is_builtin() const;
            bool is_user() const;
            bool is_complex() const;

            std::string get_name() const;

            const function* get_function(cmsl::string_view name) const;
            bool has_function(cmsl::string_view name) const;

            boost::optional<member_info> get_member(cmsl::string_view name) const;

            type_kind get_kind() const;

            bool operator==(const type& other) const;
            bool operator!=(const type& other) const;

        private:
            std::string m_name;
            type_kind m_kind;
            const ast_context* m_ast_ctx;
            const std::vector<member_info> m_members;
        };
    }
}
