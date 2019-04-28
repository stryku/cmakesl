#include "sema/sema_builder.hpp"

#include "ast/ast_node.hpp"

#include "sema/sema_builder_ast_visitor.hpp"

namespace cmsl::sema
{
        sema_builder::sema_builder(sema_context_interface& ctx,
                                   errors::errors_observer& errs,
                                   identifiers_context& ids_context,
                                   sema_type_factory& type_factory,
                                   sema_function_factory &function_factory,
                                   sema_context_factory& context_factory,
                                   add_subdirectory_handler& add_subdirectory_handler)
            : m_ctx{ ctx }
            , m_errs{ errs }
            , m_ids_context{ ids_context }
            , m_type_factory{ type_factory }
            , m_function_factory{ function_factory }
            , m_context_factory{ context_factory }
            , m_add_subdirectory_handler{ add_subdirectory_handler }
        {}

        std::unique_ptr<sema_node> sema_builder::build(const ast::ast_node &ast_tree)
        {
            sema_builder_ast_visitor visitor{ m_ctx, // generic types ctx
                                              m_ctx, // global ctx
                                              m_errs,
                                              m_ids_context,
                                              m_type_factory,
                                              m_function_factory,
                                              m_context_factory,
                                              m_add_subdirectory_handler};
            ast_tree.visit(visitor);
            return std::move(visitor.m_result_node);
        }
}
