#pragma once

#include "ast/ast_node_visitor.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class ast_node;
        class ast_context;
    }

    namespace errors
    {
        class errors_observer;
    }

    namespace sema
    {
        class sema_node;
        class identifiers_context;

        class sema_builder
        {
        public:
            explicit sema_builder(ast::ast_context& ctx, errors::errors_observer& errs, identifiers_context& ids_context);

            std::unique_ptr<sema_node> build(const ast::ast_node& ast_tree);

        private:
            ast::ast_context& m_ctx;
            errors::errors_observer& m_errs;
            identifiers_context& m_ids_context;
        };
    }
}
