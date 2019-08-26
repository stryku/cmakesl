#pragma once

#include <string>

namespace cmsl {
namespace lexer {
enum class token_type
{
  undef,
  integer,
  double_,
  string,
  dot,
  open_brace,
  close_brace,
  open_square,
  close_square,
  open_paren,
  close_paren,
  identifier,

  _operators_begin,

  equal,
  equalequal,
  minusequal,
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

  exclaimequal,
  xor_,
  xorequal,
  less,
  lessequal,
  greater,
  greaterequal,
  coloncolon,
  question,
  colon,

  _unary_operators_begin,

  plus,
  plusplus,
  minus,
  minusminus,
  exclaim,

  _unary_operators_end,

  _operators_end,

  semicolon,
  comma,

  _keywords_begin,

  _builtin_types_begin,

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

  _builtin_types_end,

  kw_return,
  kw_class,
  kw_enum,
  kw_if,
  kw_else,
  kw_while,
  kw_auto,
  kw_for,
  kw_break,
  kw_namespace,

  _keywords_end
};

bool is_builtin_type(token_type type);
bool is_unary_operator(token_type type);
}

std::string to_string(lexer::token_type type);
}
