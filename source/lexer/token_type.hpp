#pragma once

#include <string>

namespace cmsl {
namespace lexer {
enum class token_type
{
  undef,
  integer,
  double_,
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
  coloncolon,

  comma,

  kw_void,
  kw_int,
  kw_double,
  kw_bool,
  kw_true,
  kw_false,
  kw_string,
  kw_version,
  kw_list,
  kw_library,
  kw_executable,
  kw_project,

  kw_return,
  kw_class,
  kw_if,
  kw_else,
  kw_while,
  kw_auto,
  kw_for,
  kw_break,
  kw_namespace
};
}

std::string to_string(lexer::token_type type);
}
