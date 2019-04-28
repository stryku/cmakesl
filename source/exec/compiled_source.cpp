#include "exec/compiled_source.hpp"
#include "ast/ast_node.hpp"
#include "sema/sema_context.hpp"
#include "sema/sema_node.hpp"


namespace cmsl::exec
{
    compiled_source::compiled_source(std::unique_ptr<ast::ast_node> ast_tree,
                                     std::unique_ptr<sema::sema_context_interface> builtin_context,
                                     const sema::sema_context_interface& global_context,
                                     std::unique_ptr<sema::sema_node> sema_tree,
                                     source_view source)
        : m_ast_tree{ std::move(ast_tree) }
            , m_builtin_context{ std::move(builtin_context) }
            , m_global_context{ std::move(global_context) }
        , m_sema_tree{ std::move(sema_tree) }
        , m_source{ source }
    {}

    compiled_source::~compiled_source() = default;

    const sema::sema_context_interface &compiled_source::get_global_context() const
    {
        return m_global_context;
    }

    const sema::sema_function *compiled_source::get_main() const
    {
        const auto main_token = lexer::token::make_token(lexer::token::token_type::identifier, "main");
        const auto lookup_result = m_global_context.find_function(main_token);
        return lookup_result.front().front(); // Todo: handle no main function
    }
}
