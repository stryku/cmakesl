#pragma once

#include "sema/qualified_entries_finder.hpp"

#include <functional>
namespace cmsl::sema {
class sema_type;

class enum_values_context
{
protected:
  using token_t = lexer::token;
  using qualified_names_t = std::vector<ast::name_with_coloncolon>;

public:
  struct enum_value_info
  {
    std::reference_wrapper<const sema_type> type;
    unsigned value;
    unsigned index;
  };

  virtual ~enum_values_context() = default;

  virtual void register_identifier(token_t name, enum_value_info info) = 0;
  virtual std::optional<enum_value_info> info_of(
    const qualified_names_t& names) const = 0;

  virtual void enter_global_ctx(token_t name) = 0;
  virtual void leave_ctx() = 0;
};

class enum_values_context_impl : public enum_values_context
{
public:
  void register_identifier(lexer::token declaration_token,
                           enum_value_info info) override;

  std::optional<enum_value_info> info_of(
    const qualified_names_t& names) const override;

  void enter_global_ctx(token_t name) override;
  void leave_ctx() override;

private:
  qualified_entries_finder<enum_value_info> m_finder;
};

}
