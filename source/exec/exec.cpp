#include "exec/exec.hpp"

#include "lexer/lexer.hpp"
#include "errors/errors_observer.hpp"
#include "ast/ast_builder.hpp"
#include "ast/expr/expression.hpp"

#include <iostream>

namespace cmsl
{
    namespace exec
    {
        void executor::execute(cmsl::string_view source)
        {
            errors::errors_observer err_observer;
            lexer::lexer lex{ err_observer , source };
            const auto tokens = lex.lex();

            ast::ast_builder builder;
            const auto ast = builder.build(tokens);
        }
    }
}
