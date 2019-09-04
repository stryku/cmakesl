#include "token_type.hpp"

#include "common/assert.hpp"

namespace cmsl {
std::string to_string(lexer::token_type type)
{
  switch (type) {
    case lexer::token_type::integer:
      return "integer";
    case lexer::token_type::double_:
      return "double";
    case lexer::token_type::dot:
      return ".";
    case lexer::token_type::open_brace:
      return "{";
    case lexer::token_type::close_brace:
      return "}";
    case lexer::token_type::open_square:
      return "[";
    case lexer::token_type::close_square:
      return "]";
    case lexer::token_type::open_paren:
      return "(";
    case lexer::token_type::close_paren:
      return ")";
    case lexer::token_type::identifier:
      return "identifier";
    case lexer::token_type::equal:
      return "=";
    case lexer::token_type::equalequal:
      return "==";
    case lexer::token_type::minus:
      return "-";
    case lexer::token_type::minusminus:
      return "--";
    case lexer::token_type::minusequal:
      return "-=";
    case lexer::token_type::plus:
      return "+";
    case lexer::token_type::plusplus:
      return "++";
    case lexer::token_type::plusequal:
      return "+=";
    case lexer::token_type::amp:
      return "&";
    case lexer::token_type::ampamp:
      return "&&";
    case lexer::token_type::ampequal:
      return "&=";
    case lexer::token_type::pipe:
      return "|";
    case lexer::token_type::pipepipe:
      return "||";
    case lexer::token_type::pipeequal:
      return "|=";
    case lexer::token_type::slash:
      return "/";
    case lexer::token_type::slashequal:
      return "/=";
    case lexer::token_type::star:
      return "*";
    case lexer::token_type::starequal:
      return "*=";
    case lexer::token_type::percent:
      return "%";
    case lexer::token_type::percentequal:
      return "%=";
    case lexer::token_type::exclaim:
      return "!";
    case lexer::token_type::exclaimequal:
      return "!=";
    case lexer::token_type::xor_:
      return "^";
    case lexer::token_type::xorequal:
      return "^=";
    case lexer::token_type::less:
      return "<";
    case lexer::token_type::lessequal:
      return "<=";
    case lexer::token_type::greater:
      return ">";
    case lexer::token_type::greaterequal:
      return ">=";
    case lexer::token_type::string:
      return "string";
    case lexer::token_type::semicolon:
      return ";";
    case lexer::token_type::question:
      return "?";
    case lexer::token_type::colon:
      return ":";
    case lexer::token_type::coloncolon:
      return "::";
    case lexer::token_type::comma:
      return ",";
    case lexer::token_type::kw_void:
      return "void keyword";
    case lexer::token_type::kw_int:
      return "int keyword";
    case lexer::token_type::kw_double:
      return "double keyword";
    case lexer::token_type::kw_bool:
      return "bool keyword";
    case lexer::token_type::kw_true:
      return "true keyword";
    case lexer::token_type::kw_false:
      return "false keyword";
    case lexer::token_type::kw_string:
      return "string keyword";
    case lexer::token_type::kw_library:
      return "library keyword";
    case lexer::token_type::kw_executable:
      return "executable keyword";
    case lexer::token_type::kw_list:
      return "list keyword";
    case lexer::token_type::kw_return:
      return "return keyword";
    case lexer::token_type::kw_class:
      return "class keyword";
    case lexer::token_type::kw_enum:
      return "enum keyword";
    case lexer::token_type::kw_if:
      return "if keyword";
    case lexer::token_type::kw_else:
      return "else keyword";
    case lexer::token_type::kw_while:
      return "while keyword";
    case lexer::token_type::kw_auto:
      return "auto keyword";
    case lexer::token_type::kw_for:
      return "for keyword";
    case lexer::token_type::kw_break:
      return "break keyword";
    case lexer::token_type::kw_namespace:
      return "namespace keyword";
    case lexer::token_type::kw_import:
      return "import keyword";
    case lexer::token_type::kw_export:
      return "export keyword";

    case lexer::token_type::undef:
    default:
      CMSL_UNREACHABLE("Undefined token passed to to_string()");
      return "UNDEFINED";
  }
}

namespace lexer {

namespace details {
bool is_between(token_type begin, token_type end, token_type value)
{
  const auto int_value = static_cast<int>(value);

  return static_cast<int>(begin) < int_value &&
    int_value < static_cast<int>(end);
}
}

bool is_builtin_type(token_type type)
{
  return details::is_between(token_type::_builtin_types_begin,
                             token_type::_builtin_types_end, type);
}

bool is_unary_operator(token_type type)
{
  return details::is_between(token_type::_unary_operators_begin,
                             token_type::_unary_operators_end, type);
}
}

}
