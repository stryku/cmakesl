#include "sema/sema_builder.hpp"

#include "sema/sema_builder_ast_visitor.hpp"

namespace cmsl
{
    namespace sema
    {
        sema_builder::sema_builder(sema_context& ctx, errors::errors_observer& errs, identifiers_context& ids_context, sema_function_factory &function_factory)
            : m_ctx{ ctx }
            , m_errs{ errs }
            , m_ids_context{ ids_context }
        , m_function_factory{ function_factory }
        {}

        std::unique_ptr<sema_node> sema_builder::build(const ast::ast_node &ast_tree)
        {
            sema_builder_ast_visitor visitor{ m_ctx, m_errs, m_ids_context, m_function_factory };
            ast_tree.visit(visitor);
            return std::move(visitor.m_result_node);
        }
    }
}
