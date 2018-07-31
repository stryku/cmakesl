#pragma once

#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace test
    {
        namespace common
        {
            using token_t = lexer::token::token;

            token_t token_undef();
            token_t token_integer();
            token_t token_real();
            token_t token_dot();
            token_t token_open_brace();
            token_t token_close_brace();
            token_t token_open_square();
            token_t token_close_square();
            token_t token_open_paren();
            token_t token_close_paren();
            token_t token_identifier();
            token_t token_equal();
            token_t token_equalequal();
            token_t token_minus();
            token_t token_minusminus();
            token_t token_minusequal();
            token_t token_plus();
            token_t token_plusplus();
            token_t token_plusequal();
            token_t token_amp();
            token_t token_ampamp();
            token_t token_ampequal();
            token_t token_pipe();
            token_t token_pipepipe();
            token_t token_pipeequal();
            token_t token_slash();
            token_t token_slashequal();
            token_t token_star();
            token_t token_starequal();
            token_t token_percent();
            token_t token_percentequal();
            token_t token_exclaim();
            token_t token_exclaimequal();
            token_t token_xor();
            token_t token_xorequal();
            token_t token_string();
            token_t token_semicolon();
            token_t token_t_int();
            token_t token_t_real();
        }
    }
}