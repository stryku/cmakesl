#pragma once

#include "ast/qualified_name.hpp"
#include "common/algorithm.hpp"
#include "common/assert.hpp"
#include "lexer/token.hpp"
#include "sema/qualified_entries_finder.hpp"

#include <algorithm>
#include <functional>

namespace cmsl::sema {
class sema_type;

class identifiers_context
{
protected:
  using token_t = lexer::token;
  using qualified_names_t = std::vector<ast::name_with_coloncolon>;

public:
  struct identifier_info
  {
    std::reference_wrapper<const sema_type> type;
    unsigned index;
  };

  virtual ~identifiers_context() = default;

  virtual void register_identifier(token_t name, identifier_info info) = 0;
  virtual std::optional<identifier_info> info_of(
    const qualified_names_t& names) const = 0;

  virtual void enter_global_ctx(token_t name) = 0;
  virtual void enter_local_ctx() = 0;
  virtual void leave_ctx() = 0;
};

class identifiers_context_impl : public identifiers_context
{
public:
  void register_identifier(lexer::token declaration_token,
                           identifier_info info) override;

  std::optional<identifier_info> info_of(
    const qualified_names_t& names) const override;
  std::optional<lexer::token> declaration_token_of(
    const qualified_names_t& names) const;

  std::optional<lexer::token> declaration_token_of_ctx(
    const qualified_names_t& names) const;

  void enter_global_ctx(token_t name) override;
  void enter_local_ctx() override;
  void leave_ctx() override;

private:
  qualified_entries_finder<identifier_info> m_contextes_handler;
};
}
