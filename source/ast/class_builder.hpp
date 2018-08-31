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
        class function_node;

        class class_builder
        {
        public:
            explicit class_builder(ast_context& parent_ast_ctx, cmsl::string_view name);
            ~class_builder();

            class_builder& with_member(member_declaration member);
            class_builder& with_function(std::unique_ptr<function_node> fun);

            std::unique_ptr<class_node> build();

        private:
            std::unique_ptr<ast_context> m_ast_ctx;
            cmsl::string_view m_name;
            std::vector<member_declaration> m_members;
        };
    }
}
