#include "lexer/lexer.hpp"
#include "errors/error.hpp"
#include "errors/errors_observer.hpp"

#include <cassert>
#include <cctype>

namespace cmsl::lexer {
lexer::lexer(errors::errors_observer& err_observer, source_t source)
  : m_err_observer{ err_observer }
  , m_source{ source }
  , m_source_loc{ m_source.source() }
  , m_aritmetical_token_definitions{ create_arithmetical_token_definitions() }
  , m_one_char_tokens{ create_one_char_tokens() }
  , m_keyword_tokens{ create_keyword_tokens() }
{
}

std::vector<token> lexer::lex()
{
  auto tokens = std::vector<token>{};

  while (!is_end()) {
    const auto t = get_next_token();

    if (t.get_type() != token_type::undef) {
      tokens.push_back(t);
    }
  }

  return tokens;
}

bool lexer::is_end() const
{
  return m_source_loc.is_at_end();
}

bool lexer::has_next() const
{
  return m_source_loc.has_next();
}

char lexer::current() const
{
  return m_source_loc.current_char();
}

char lexer::next() const
{
  return m_source_loc.next_char();
}

token lexer::get_next_token()
{
  consume_whitespaces();
  const auto begin_loc = m_source_loc.location();
  const auto token_type = get_next_token_type();
  const auto end_loc = m_source_loc.location();
  return token{ token_type, source_range{ begin_loc, end_loc }, m_source };
}

token_type lexer::get_next_token_type()
{
  if (is_end()) {
    // in such case, there are whitespaces at the end of source. No other chars
    return token_type::undef;
  }

  const auto curr = current();

  if (std::isdigit(curr)) {
    return get_numeric_token_type();
  }
  if (curr == '.') {
    if (has_next() && std::isdigit(next())) // .123
    {
      return get_numeric_token_type();
    } else {
      consume_char();
      return token_type::dot;
    }
  }
  if (curr == '=') {
    return get_equal_token_type();
  }
  if (curr == '"') {
    return get_string_token_type();
  }
  if (is_one_char_token(curr)) {
    return get_one_char_token_type(curr);
  }
  if (is_arithmetical_operator(curr)) {
    return get_arithmetical_token_type(curr);
  }
  if (std::isalpha(curr) || curr == '_') {
    return get_identifier_or_keyword_token_type();
  }

  return token_type::undef;
}

token_type lexer::get_numeric_token_type()
{
  consume_integer();

  if (is_end()) {
    return token_type::integer;
  }

  // Handle double_ e.g. 123.
  if (current() == '.') {
    m_source_loc.consume_char();
    consume_integer();
    return token_type::double_;
  }

  // TODO: handle hex, oct and bin

  return token_type::integer;
}

void lexer::consume_integer()
{
  while (!is_end() && std::isdigit(current())) {
    consume_char();
  }
}

void lexer::consume_char()
{
  assert(!is_end());
  m_source_loc.consume_char();
}

void lexer::consume_whitespaces()
{
  assert(!is_end());
  while (!is_end() && is_whitespace(current())) {
    consume_char();
  }
}

token_type lexer::get_identifier_or_keyword_token_type()
{
  std::string token_val;
  while (!is_end() && is_identifier_char(current())) {
    token_val += current();
    consume_char();
  }

  auto found = m_keyword_tokens.find(token_val);

  if (found != m_keyword_tokens.end()) {
    return found->second;
  }

  return found != m_keyword_tokens.end() ? found->second
                                         : token_type::identifier;
}

bool lexer::is_identifier_char(char c) const
{
  // At this point we know that identifier won't start with a digit, so we can
  // use isalnum()
  return std::isalnum(c) || c == '_';
}

token_type lexer::get_equal_token_type()
{
  // current == '='
  m_source_loc.consume_char();

  if (!is_end() && current() == '=') // ==
  {
    consume_char();
    return token_type::equalequal;
  }

  // =
  return token_type::equal;
}

token_type lexer::get_string_token_type()
{
  consume_char();

  // TODO: handle new lines
  while (!is_end() && current() != '"') {
    if (current() == '\\') {
      // Doesn't matter what is after '\', we need to consume it
      consume_char();

      if (is_end()) {
        break;
      }
    }

    consume_char();
  }

  if (is_end()) {
    const auto current_loc = m_source_loc.location();

    errors::error err;
    err.type = errors::error_type::error;
    err.source_path = m_source.path();
    err.range = source_range{ current_loc, current_loc };
    err.message = "Unexpected end of source in a middle of string";

    const auto line_info = m_source.line(current_loc.line);
    err.line_snippet = line_info.line;
    err.line_start_pos = line_info.start_pos;

    m_err_observer.nofify_error(std::move(err));
    return token_type::undef;
  }

  // current() == '"', consume
  consume_char();

  return token_type::string;
}

token_type lexer::get_arithmetical_token_type(char operator_char)
{
  // current() == operator_char, go to next char
  consume_char();

  const auto found = m_aritmetical_token_definitions.find(operator_char);
  assert(found != m_aritmetical_token_definitions.end());

  const auto& definition = found->second;

  if (is_end()) {
    return definition.single;
  }

  if (definition.has_twice() && current() == operator_char) // same, e.g. ++
  {
    consume_char();
    return definition.twice;
  }

  if (current() == '=') // e.g. +=
  {
    consume_char();
    return definition.op_equal;
  }

  return definition.single;
}

lexer::aritmetical_token_definition_t
lexer::create_arithmetical_token_definitions() const
{
  using def_t = arithmetical_token_definition;

  aritmetical_token_definition_t definitions;

  definitions['-'] =
    def_t{ token_type::minus, token_type::minusequal, token_type::minusminus };
  definitions['+'] =
    def_t{ token_type::plus, token_type::plusequal, token_type::plusplus };
  definitions['&'] =
    def_t{ token_type::amp, token_type::ampequal, token_type::ampamp };
  definitions['|'] =
    def_t{ token_type::pipe, token_type::pipeequal, token_type::pipepipe };
  definitions['/'] = def_t{ token_type::slash, token_type::slashequal };
  definitions['*'] = def_t{ token_type::star, token_type::starequal };
  definitions['%'] = def_t{ token_type::percent, token_type::percentequal };
  definitions['!'] = def_t{ token_type::exclaim, token_type::exclaimequal };
  definitions['^'] = def_t{ token_type::xor_, token_type::xorequal };
  definitions['<'] = def_t{ token_type::less, token_type::lessequal };
  definitions['>'] = def_t{ token_type::greater, token_type::greaterequal };

  return definitions;
}

lexer::one_char_tokens_t lexer::create_one_char_tokens() const
{
  one_char_tokens_t tokens;

  tokens['('] = token_type::open_paren;
  tokens[')'] = token_type::close_paren;
  tokens['{'] = token_type::open_brace;
  tokens['}'] = token_type::close_brace;
  tokens['['] = token_type::open_square;
  tokens[']'] = token_type::close_square;
  tokens[';'] = token_type::semicolon;
  tokens[','] = token_type::comma;

  return tokens;
}

bool lexer::is_arithmetical_operator(char c) const
{
  const auto arith_operators = cmsl::string_view{ "-+&|/*%!^<>" };
  return arith_operators.find(c) != cmsl::string_view::npos;
}

bool lexer::is_one_char_token(char c) const
{
  const auto chars = cmsl::string_view{ "{}[]();," };
  return chars.find(c) != cmsl::string_view::npos;
}

bool lexer::is_whitespace(char c) const
{
  const auto chars = cmsl::string_view{ " \t\n\r" };
  return chars.find(c) != cmsl::string_view::npos;
}

token_type lexer::get_one_char_token_type(char c)
{
  const auto found = m_one_char_tokens.find(c);
  assert(found != m_one_char_tokens.end());

  consume_char();

  const auto token_type = found->second;
  return token_type;
}

lexer::keyword_tokens_t lexer::create_keyword_tokens() const
{
  keyword_tokens_t tokens;

  tokens["void"] = token_type::kw_void;
  tokens["int"] = token_type::kw_int;
  tokens["double"] = token_type::kw_double;
  tokens["bool"] = token_type::kw_bool;
  tokens["true"] = token_type::kw_true;
  tokens["false"] = token_type::kw_false;
  tokens["string"] = token_type::kw_string;
  tokens["version"] = token_type::kw_version;
  tokens["list"] = token_type::kw_list;
  tokens["library"] = token_type::kw_library;
  tokens["executable"] = token_type::kw_executable;
  tokens["project"] = token_type::kw_project;
  tokens["return"] = token_type::kw_return;
  tokens["class"] = token_type::kw_class;
  tokens["if"] = token_type::kw_if;
  tokens["else"] = token_type::kw_else;
  tokens["while"] = token_type::kw_while;
  tokens["auto"] = token_type::kw_auto;
  tokens["for"] = token_type::kw_for;
  tokens["break"] = token_type::kw_break;

  return tokens;
}
}
