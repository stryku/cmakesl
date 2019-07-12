#pragma once

#include "lexer/token.hpp"

namespace cmsl::test::common {
using token_t = lexer::token;

token_t token_undef();
token_t token_integer(cmsl::string_view str = "");
token_t token_double(cmsl::string_view str = "");
token_t token_dot();
token_t token_open_brace();
token_t token_close_brace();
token_t token_open_square();
token_t token_close_square();
token_t token_open_paren();
token_t token_close_paren();
token_t token_identifier(cmsl::string_view str = "");
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
token_t token_string(cmsl::string_view str);
token_t token_semicolon();
token_t token_coloncolon();
token_t token_comma();
token_t token_less();
token_t token_lessequal();
token_t token_greater();
token_t token_greaterequal();

token_t token_kw_void();
token_t token_kw_int();
token_t token_kw_double();
token_t token_kw_return();
token_t token_kw_class();
token_t token_kw_if();
token_t token_kw_else();
token_t token_kw_while();
token_t token_kw_bool();
token_t token_kw_true();
token_t token_kw_false();
token_t token_kw_string();
token_t token_kw_list();
token_t token_kw_project();
token_t token_kw_library();
token_t token_kw_executable();
token_t token_kw_auto();
token_t token_kw_for();
token_t token_kw_break();
token_t token_kw_namespace();

token_t token_from_larger_source(cmsl::string_view source,
                                 lexer::token_type type, unsigned begin,
                                 unsigned end);
}