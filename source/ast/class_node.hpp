#pragma once

#include "ast/ast_context.hpp"
#include "ast/ast_node.hpp"
#include "ast/type.hpp"
#include "common/string.hpp"
#include "ast/member_declaration.hpp"

namespace cmsl
{
    namespace ast
    {
        class ast_context;

        class class_node : public ast_node
        {
        public:
            class_node(std::unique_ptr<ast_context> ast_ctx, cmsl::string_view name, std::vector<member_declaration> m_members);

            cmsl::string_view get_name() const;
            const std::vector<member_declaration>& get_members() const;
            const ast_context& get_ast_ctx() const;

        private:
            std::unique_ptr<ast_context> m_ast_ctx;
            cmsl::string_view m_name;
            std::vector<member_declaration> m_members;
        };
    }
}
