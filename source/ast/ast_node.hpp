#pragma once

#include "ast/ast_node_type.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    struct source_location;
    
    namespace ast
    {
        class ast_node_visitor;

        class ast_node
        {
        public:
            using token_t = lexer::token::token;

            ast_node()
            {}

            ast_node(ast_node_type type)
                    : m_type{type}
            {}

            virtual ~ast_node()
            {}

            ast_node_type get_type() const
            {
                return m_type;
            }

            virtual void visit(ast_node_visitor &visitor) const = 0;
            virtual source_location begin_location() const = 0;
            virtual source_location end_location() const = 0;

        private:
            ast_node_type m_type;
        };
    }
}
