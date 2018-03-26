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
                real,
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
                xor,
                xorequal,
                string
            };
        }
    }
}
