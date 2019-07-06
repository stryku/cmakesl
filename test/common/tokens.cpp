#include "test/common/tokens.hpp"
#include "common/enum_class_utils.hpp"

namespace cmsl::test::common {
using token_type_t = lexer::token_type;
using token_t = lexer::token;

namespace details {
using tokens_str_t = enum_unordered_map<token_type_t, cmsl::string_view>;

const tokens_str_t& operators_str()
{
  static tokens_str_t map = { { token_type_t::dot, "." },
                              { token_type_t::open_brace, "{" },
                              { token_type_t::close_brace, "}" },
                              { token_type_t::open_square, "[" },
                              { token_type_t::close_square, "]" },
                              { token_type_t::open_paren, "(" },
                              { token_type_t::close_paren, ")" },
                              { token_type_t::equal, "=" },
                              { token_type_t::equalequal, "==" },
                              { token_type_t::minus, "-" },
                              { token_type_t::minusminus, "--" },
                              { token_type_t::minusequal, "-=" },
                              { token_type_t::plus, "+" },
                              { token_type_t::plusplus, "++" },
                              { token_type_t::plusequal, "+=" },
                              { token_type_t::amp, "&" },
                              { token_type_t::ampamp, "&&" },
                              { token_type_t::ampequal, "&=" },
                              { token_type_t::pipe, "|" },
                              { token_type_t::pipepipe, "||" },
                              { token_type_t::pipeequal, "|=" },
                              { token_type_t::slash, "/" },
                              { token_type_t::slashequal, "/=" },
                              { token_type_t::star, "*" },
                              { token_type_t::starequal, "*=" },
                              { token_type_t::percent, "%" },
                              { token_type_t::percentequal, "%=" },
                              { token_type_t::exclaim, "!" },
                              { token_type_t::exclaimequal, "!=" },
                              { token_type_t::xor_, "^" },
                              { token_type_t::xorequal, "^=" },
                              { token_type_t::less, "<" },
                              { token_type_t::lessequal, "<=" },
                              { token_type_t::greater, ">" },
                              { token_type_t::greaterequal, ">=" },
                              { token_type_t::semicolon, ";" },
                              { token_type_t::comma, "," },
                              { token_type_t::kw_void, "void" },
                              { token_type_t::kw_int, "int" },
                              { token_type_t::kw_double, "double" },
                              { token_type_t::kw_return, "return" },
                              { token_type_t::kw_class, "class" },
                              { token_type_t::kw_if, "if" },
                              { token_type_t::kw_else, "else" },
                              { token_type_t::kw_while, "while" },
                              { token_type_t::kw_bool, "bool" },
                              { token_type_t::kw_true, "true" },
                              { token_type_t::kw_false, "false" },
                              { token_type_t::kw_string, "string" },
                              { token_type_t::kw_library, "library" },
                              { token_type_t::kw_executable, "executable" },
                              { token_type_t::kw_list, "list" },
                              { token_type_t::kw_project, "project" },
                              { token_type_t::kw_auto, "auto" },
                              { token_type_t::kw_for, "for" },
                              { token_type_t::kw_break, "break" } };

  return map;
}

token_t token_from_source(token_type_t type, cmsl::string_view source)
{
  const auto begin = source_location{};
  source_location end;
  end.line = 1u;
  end.column = source.size();
  end.absolute = source.size();
  const auto source_range = cmsl::source_range{ begin, end };

  return token_t{ type, source_range, cmsl::source_view{ source } };
}

token_t simple_token(token_type_t type)
{
  const auto& map = operators_str();
  const auto source = map.at(type);
  return token_from_source(type, source);
}
}

token_t token_dot()
{
  return details::simple_token(token_type_t::dot);
}

token_t token_open_brace()
{
  return details::simple_token(token_type_t::open_brace);
}

token_t token_close_brace()
{
  return details::simple_token(token_type_t::close_brace);
}

token_t token_open_square()
{
  return details::simple_token(token_type_t::open_square);
}

token_t token_close_square()
{
  return details::simple_token(token_type_t::close_square);
}

token_t token_open_paren()
{
  return details::simple_token(token_type_t::open_paren);
}

token_t token_close_paren()
{
  return details::simple_token(token_type_t::close_paren);
}

token_t token_equal()
{
  return details::simple_token(token_type_t::equal);
}

token_t token_equalequal()
{
  return details::simple_token(token_type_t::equalequal);
}

token_t token_minus()
{
  return details::simple_token(token_type_t::minus);
}

token_t token_minusminus()
{
  return details::simple_token(token_type_t::minusminus);
}

token_t token_minusequal()
{
  return details::simple_token(token_type_t::minusequal);
}

token_t token_plus()
{
  return details::simple_token(token_type_t::plus);
}

token_t token_plusplus()
{
  return details::simple_token(token_type_t::plusplus);
}

token_t token_plusequal()
{
  return details::simple_token(token_type_t::plusequal);
}

token_t token_amp()
{
  return details::simple_token(token_type_t::amp);
}

token_t token_ampamp()
{
  return details::simple_token(token_type_t::ampamp);
}

token_t token_ampequal()
{
  return details::simple_token(token_type_t::ampequal);
}

token_t token_pipe()
{
  return details::simple_token(token_type_t::pipe);
}

token_t token_pipepipe()
{
  return details::simple_token(token_type_t::pipepipe);
}

token_t token_pipeequal()
{
  return details::simple_token(token_type_t::pipeequal);
}

token_t token_slash()
{
  return details::simple_token(token_type_t::slash);
}

token_t token_slashequal()
{
  return details::simple_token(token_type_t::slashequal);
}

token_t token_star()
{
  return details::simple_token(token_type_t::star);
}

token_t token_starequal()
{
  return details::simple_token(token_type_t::starequal);
}

token_t token_percent()
{
  return details::simple_token(token_type_t::percent);
}

token_t token_percentequal()
{
  return details::simple_token(token_type_t::percentequal);
}

token_t token_exclaim()
{
  return details::simple_token(token_type_t::exclaim);
}

token_t token_exclaimequal()
{
  return details::simple_token(token_type_t::exclaimequal);
}

token_t token_xor()
{
  return details::simple_token(token_type_t::xor_);
}

token_t token_xorequal()
{
  return details::simple_token(token_type_t::xorequal);
}

token_t token_less()
{
  return details::simple_token(token_type_t::less);
}

token_t token_lessequal()
{
  return details::simple_token(token_type_t::lessequal);
}

token_t token_greater()
{
  return details::simple_token(token_type_t::greater);
}

token_t token_greaterequal()
{
  return details::simple_token(token_type_t::greaterequal);
}

token_t token_semicolon()
{
  return details::simple_token(token_type_t::semicolon);
}

token_t token_comma()
{
  return details::simple_token(token_type_t::comma);
}

token_t token_kw_void()
{
  return details::simple_token(token_type_t::kw_void);
}

token_t token_kw_int()
{
  return details::simple_token(token_type_t::kw_int);
}

token_t token_kw_double()
{
  return details::simple_token(token_type_t::kw_double);
}

token_t token_kw_return()
{
  return details::simple_token(token_type_t::kw_return);
}

token_t token_kw_class()
{
  return details::simple_token(token_type_t::kw_class);
}

token_t token_kw_if()
{
  return details::simple_token(token_type_t::kw_if);
}

token_t token_kw_else()
{
  return details::simple_token(token_type_t::kw_else);
}

token_t token_kw_while()
{
  return details::simple_token(token_type_t::kw_while);
}

token_t token_kw_bool()
{
  return details::simple_token(token_type_t::kw_bool);
}

token_t token_kw_true()
{
  return details::simple_token(token_type_t::kw_true);
}

token_t token_kw_false()
{
  return details::simple_token(token_type_t::kw_false);
}

token_t token_kw_string()
{
  return details::simple_token(token_type_t::kw_string);
}

token_t token_kw_library()
{
  return details::simple_token(token_type_t::kw_library);
}

token_t token_kw_executable()
{
  return details::simple_token(token_type_t::kw_executable);
}

token_t token_kw_list()
{
  return details::simple_token(token_type_t::kw_list);
}

token_t token_kw_project()
{
  return details::simple_token(token_type_t::kw_project);
}

token_t token_kw_auto()
{
  return details::simple_token(token_type_t::kw_auto);
}

token_t token_kw_for()
{
  return details::simple_token(token_type_t::kw_for);
}

token_t token_kw_break()
{
  return details::simple_token(token_type_t::kw_break);
}

token_t token_undef()
{
  return token_t{ token_type_t::undef };
}

token_t token_double(cmsl::string_view str)
{
  return details::token_from_source(token_type_t::double_, str);
}

token_t token_string(cmsl::string_view str)
{
  return details::token_from_source(token_type_t::string, str);
}

token_t token_integer(cmsl::string_view str)
{
  return details::token_from_source(token_type_t::integer, str);
}

token_t token_identifier(cmsl::string_view str)
{
  return details::token_from_source(token_type_t::identifier, str);
}

token_t token_from_larger_source(cmsl::string_view source, token_type_t type,
                                 unsigned begin, unsigned end)
{
  source_location begin_loc;
  begin_loc.line = 1u;
  begin_loc.column = begin_loc.absolute = begin;

  source_location end_loc;
  end_loc.line = 1u;
  end_loc.column = end_loc.absolute = end;
  const auto source_range = cmsl::source_range{ begin_loc, end_loc };

  return token_t{ type, source_range, cmsl::source_view{ source } };
}
}