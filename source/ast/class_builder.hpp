#pragma once

#include "ast/member_declaration.hpp"
#include "common/string.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class class_node;
        class ast_context;
        class function;

        class class_builder
        {
        public:
            explicit class_builder(ast_context& parent_ast_ctx, cmsl::string_view name);
            ~class_builder();

            class_builder& with_member(member_declaration member);
            class_builder& with_function(std::unique_ptr<function> fun);

            std::unique_ptr<class_node> build();

            ast_context& get_ast_ctx();

        private:
            std::unique_ptr<ast_context> m_ast_ctx;
            cmsl::string_view m_name;
            std::vector<member_declaration> m_members;
            ast::type_kind m_kind;
        };
    }
}
