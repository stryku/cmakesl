#pragma once

#include "ast/type.hpp"
#include "ast/class_node.hpp"
#include "type_kind.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class class_type : public type
        {
        public:
            class_type(std::unique_ptr<class_node> node, type_kind kind = type_kind::k_user);

            const std::vector<member_declaration>& get_members_decl() const;

        private:
            std::unique_ptr<class_node> m_node;
        };

        class class_type2 : public type
        {
        public:
            class_type2(lexer::token::token name, std::unique_ptr<ast_context> ctx, std::vector<member_info> members, type_kind kind = type_kind::k_user)
                : type{ name.str(), kind, ctx.get(), std::move(members) }
                , m_ctx{ std::move(ctx) }
            {}

        private:
            std::unique_ptr<ast_context> m_ctx;
        };
    }
}
