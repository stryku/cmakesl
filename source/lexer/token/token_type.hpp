#pragma once

namespace cmsl
{
    namespace lexer
    {
        namespace token
        {
            enum class token_type
            {
                undef,
                integer,
                real, // todo rename to double
                dot,
                open_brace,
                close_brace,
                open_square,
                close_square,
                open_paren,
                close_paren,
                identifier,
                equal,
                equalequal,
                minus,
                minusminus,
                minusequal,
                plus,
                plusplus,
                plusequal,
                amp,
                ampamp,
                ampequal,
                pipe,
                pipepipe,
                pipeequal,
                slash,
                slashequal,
                star,
                starequal,
                percent,
                percentequal,
                exclaim,
                exclaimequal,
                xor_,
                xorequal,
                less,
                lessequal,
                greater,
                greaterequal,
                string,
                semicolon,
                comma,

                // builtin types
                t_int, // todo rename to kw_*
                t_real,

                kw_return,
                kw_class,
                kw_if,
                kw_else,
                kw_while
            };
        }
    }
}
