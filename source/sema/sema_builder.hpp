#pragma once

#include "ast/ast_node_visitor.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class ast_node;
    }

    namespace errors
    {
        class errors_observer;
    }

    namespace sema
    {
        class sema_context;
        class sema_node;
        class identifiers_context;
        class sema_function_factory;

        class sema_builder
        {
        public:
            explicit sema_builder(sema_context& ctx, errors::errors_observer& errs, identifiers_context& ids_context, sema_function_factory &function_factory);

            std::unique_ptr<sema_node> build(const ast::ast_node& ast_tree);

        private:
            sema_context& m_ctx;
            errors::errors_observer& m_errs;
            identifiers_context& m_ids_context;
            sema_function_factory &m_function_factory;
        };
    }
}
