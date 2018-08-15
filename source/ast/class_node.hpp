#pragma once

#include "ast/ast_node.hpp"
#include "ast/type.hpp"
#include "common/string.hpp"
#include "ast/member_declaration.hpp"

namespace cmsl
{
    namespace ast
    {
        class class_node : public ast_node
        {
        public:
            class_node(cmsl::string_view name, std::vector<member_declaration> m_members);

            cmsl::string_view get_name() const;

        private:
            cmsl::string_view m_name;
            std::vector<member_declaration> m_members;
        };
    }
}
