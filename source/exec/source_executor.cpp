#include "exec/source_executor.hpp"

#include "lexer/lexer.hpp"
#include "errors/errors_observer.hpp"
#include "ast/ast_builder.hpp"
#include "ast/builtin_ast_context.hpp"
#include "exec/execution.hpp"
#include "exec/execution2.hpp"
#include "ast/parser2.hpp"
#include "sema/sema_builder.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/sema_node.hpp"
#include "sema/factories.hpp"
#include "sema/builtin_sema_context.hpp"
#include "sema/sema_function.hpp"
#include "sema/user_sema_function.hpp"
#include "common/source_view.hpp"
#include "source_executor.hpp"


namespace cmsl
{
    // Todo: remove
    template<unsigned N>
    lexer::token::token make_token(lexer::token::token_type token_type, const char (&tok)[N])
    {
        // N counts also '\0'
        const auto src_range = source_range{
                source_location{ 1u, 1u, 0u },
                source_location{ 1u, N, N - 1u }
        };
        return lexer::token::token{ token_type, src_range, cmsl::source_view{ tok } };
    }

    namespace exec
    {
        source_executor::source_executor(facade::cmake_facade &f)
            : m_cmake_facade{ f }
        {}

        int source_executor::execute(cmsl::string_view source)
        {
            errors::errors_observer err_observer;
            lexer::lexer lex{ err_observer , cmsl::source_view{ source } };
            const auto tokens = lex.lex();

            ast::ast_builder builder;
            ast::builtin_ast_context ctx;
            auto global_ast_ctx = builder.build(ctx, err_observer, tokens);
            const auto &tt = ctx.find_type("string");
            const auto main_function = global_ast_ctx->find_function("main");
            const auto casted = dynamic_cast<const ast::user_function_node*>(main_function);

            execution e{ m_cmake_facade };
            e.function_call(*casted, {});
            const auto main_result = e.get_function_return_value();
            const auto int_result = main_result->get_value_cref().get_int();
            return int_result;
        }
    }
}
