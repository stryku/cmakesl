#include "exec/source_executor.hpp"

#include "lexer/lexer.hpp"
#include "errors/errors_observer.hpp"
#include "ast/ast_builder.hpp"
#include "ast/builtin_ast_context.hpp"
#include "exec/execution.hpp"

namespace cmsl
{
    namespace exec
    {
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

            execution e;
            e.function_call(*casted, {});
            const auto main_result = e.get_function_return_value();
            const auto int_result = boost::get<inst::int_t>(main_result);
            return int_result;
        }
    }
}
