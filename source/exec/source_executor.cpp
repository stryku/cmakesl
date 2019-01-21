#include "exec/source_executor.hpp"

#include "lexer/lexer.hpp"
#include "errors/errors_observer.hpp"
#include "ast/ast_builder.hpp"
#include "ast/builtin_ast_context.hpp"
#include "exec/execution.hpp"
#include "ast/parser2.hpp"
#include "sema/sema_builder.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/sema_node.hpp"

namespace cmsl
{
    namespace exec
    {
        source_executor::source_executor(facade::cmake_facade &f)
            : m_cmake_facade{ f }
        {}

        int source_executor::execute2(cmsl::string_view source)
        {
            errors::errors_observer err_observer;
            lexer::lexer lex{ err_observer , source };
            const auto tokens = lex.lex();

            ast::parser2 parser{ err_observer, tokens };
            auto ast_tree = parser.translation_unit();

            ast::builtin_ast_context ctx;
            sema::identifiers_context_impl ids_ctx;
            sema::sema_builder sema_builder{ ctx, err_observer, ids_ctx };
            auto sema_tree = sema_builder.build(*ast_tree);


            const auto main_function = ctx.find_function("main");
            const auto casted = dynamic_cast<const ast::user_function_node2*>(main_function);

            execution e{ m_cmake_facade };




        }

        int source_executor::execute(cmsl::string_view source)
        {
            errors::errors_observer err_observer;
            lexer::lexer lex{ err_observer , source };
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
            const auto int_result = boost::get<inst::int_t>(main_result->get_value());
            return int_result;
        }
    }
}
