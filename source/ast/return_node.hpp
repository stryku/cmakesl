#pragma once

#include "ast/ast_node.hpp"

#include <memory>

namespace cmsl::ast
{
        class return_node : public ast_node
        {
        public:
            explicit return_node(token_t return_kw, std::unique_ptr<ast_node> expr, token_t semicolon);

            //todo: remove get_
            const ast_node& get_expression() const;

            token_t return_kw() const;
            token_t semicolon() const;

            void visit(ast_node_visitor &visitor) const override;

            source_location begin_location() const override;
            source_location end_location() const override;

        private:
            token_t m_return_kw;
            std::unique_ptr<ast_node> m_expression;
            token_t m_semicolon;
        };
}
