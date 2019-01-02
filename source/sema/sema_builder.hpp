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
        class sema_node;

        class sema_builder
        {
        public:
            explicit sema_builder(errors::errors_observer& errs);

            std::unique_ptr<sema_node> build(const ast::ast_node& ast_tree);

        private:
            errors::errors_observer& m_errs;
        };
    }
}
