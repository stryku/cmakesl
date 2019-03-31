#include "exec/compiled_source.hpp"
#include "compiled_source.hpp"


namespace cmsl::exec
{
    compiled_source::compiled_source(std::unique_ptr<ast::ast_node> ast_tree,
                                     std::unique_ptr<sema::sema_context_interface> global_context,
                                     std::unique_ptr<sema::sema_node> sema_tree,
                                     source_view source)
        : m_ast_tree{ std::move(ast_tree) }
        , m_global_context{ std::move(global_context) }
        , m_sema_tree{ std::move(sema_tree) }
        , m_source{ source }
    {}
}
