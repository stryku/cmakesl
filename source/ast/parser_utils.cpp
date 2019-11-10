#include "ast/parser_utils.hpp"

#include "ast/parse_errors_observer.hpp"
#include "common/algorithm.hpp"
#include "parser_utils.hpp"

namespace cmsl::ast {
parser_utils::parser_utils(parse_errors_observer& err_observer,
                           token_it current, token_it end)
  : parse::parser_utils{ err_observer, current, end }
{
}

std::optional<std::vector<ast::name_with_coloncolon>>
parser_utils::parse_possibly_qualified_name()
{
  std::vector<ast::name_with_coloncolon> names;

  const auto starts_with_global_scope_access =
    current_is(token_type_t::coloncolon);
  if (starts_with_global_scope_access) {
    auto name = lexer::make_token(token_type_t::identifier, "");
    auto coloncolon = eat(token_type_t::coloncolon);

    names.emplace_back(name_with_coloncolon{ name, coloncolon });
  }

  while (true) {
    token_t name;
    if (current_is(token_type_t::identifier)) {
      name = *eat(token_type_t::identifier);
    } else if (current_is(token_type_t::kw_auto)) {
      name = *eat();
    } else if (is_builtin_type(curr_type())) {
      name = *eat();
    } else {
      return std::nullopt;
    }

    if (auto coloncolon = try_eat(token_type_t::coloncolon)) {
      names.emplace_back(name_with_coloncolon{ name, coloncolon });
    } else {
      names.emplace_back(name_with_coloncolon{ name });
      break;
    }
  }

  return std::move(names);
}

bool parser_utils::possibly_qualified_name_starts() const
{
  return current_is(token_type_t::identifier) ||
    current_is(token_type_t::coloncolon);
}

}
