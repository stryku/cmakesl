#pragma once

#include "ast/ast_node.hpp"
#include "ast/type.hpp"
#include "ast/infix_node.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class declaration_node : public ast_node
        {
        public:
            declaration_node(const type& t, cmsl::string_view name, std::unique_ptr<infix_node> expr);

            const type& get_declared_type() const;
            cmsl::string_view get_name() const;
            const infix_node* get_expression() const;

        private:
            const type& m_type;
            cmsl::string_view m_name;
            std::unique_ptr<infix_node> m_expression;
        };
    }
}
