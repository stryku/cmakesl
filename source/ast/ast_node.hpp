#pragma once

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

            virtual ~ast_node()
            {}

            virtual void visit(ast_node_visitor &visitor) const = 0;
            virtual source_location begin_location() const = 0;
            virtual source_location end_location() const = 0;
        };
    }
}
