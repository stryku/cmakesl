#pragma once

#include "ast/qualified_name.hpp"
#include "common/algorithm.hpp"
#include "common/assert.hpp"
#include "lexer/token.hpp"
#include "sema/identifier_info.hpp"
#include "sema/qualified_entries_finder.hpp"

#include <functional>
#include <memory>

namespace cmsl {
namespace errors {
class errors_observer;
}

namespace sema {
class sema_type;

class identifiers_context
{
protected:
  using token_t = lexer::token;
  using qualified_names_t = std::vector<ast::name_with_coloncolon>;

public:
  virtual ~identifiers_context() = default;

  virtual void register_identifier(token_t name, identifier_info info,
                                   bool exported) = 0;
  virtual std::optional<identifier_info> info_of(
    const qualified_names_t& names) const = 0;

  virtual void enter_global_ctx(token_t name, bool exported) = 0;
  virtual void enter_local_ctx() = 0;
  virtual void leave_ctx() = 0;
  virtual bool is_in_global_ctx() const = 0;

  virtual std::unique_ptr<identifiers_context> clone() const = 0;
  virtual std::unique_ptr<identifiers_context> collect_exported_stuff()
    const = 0;

  virtual bool merge_imported_stuff(const identifiers_context& imported,
                                    errors::errors_observer& errs) = 0;
};

class identifiers_context_impl : public identifiers_context
{
public:
  void register_identifier(lexer::token declaration_token,
                           identifier_info info, bool exported) override;

  std::optional<identifier_info> info_of(
    const qualified_names_t& names) const override;
  std::optional<lexer::token> declaration_token_of(
    const qualified_names_t& names) const;

  std::optional<lexer::token> declaration_token_of_ctx(
    const qualified_names_t& names) const;

  void enter_global_ctx(token_t name, bool exported) override;
  void enter_local_ctx() override;
  void leave_ctx() override;
  bool is_in_global_ctx() const override;

  std::unique_ptr<identifiers_context> clone() const override;
  std::unique_ptr<identifiers_context> collect_exported_stuff() const override;

  bool merge_imported_stuff(const identifiers_context& imported,
                            errors::errors_observer& errs) override;

private:
  qualified_entries_finder<identifier_info> m_contextes_handler;
};
}
}
