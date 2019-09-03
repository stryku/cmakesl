#pragma once

#include "sema/qualified_entries_finder.hpp"

#include <memory>

namespace cmsl::sema {
class sema_type;

class types_context
{
public:
  virtual ~types_context() = default;

  virtual void register_type(const lexer::token& name,
                             const sema_type& ty) = 0;

  // Returns the non-reference type.
  virtual const sema_type* find(
    const std::vector<ast::name_with_coloncolon>& names) const = 0;

  virtual const sema_type* find_in_current_scope(
    const lexer::token& name) const = 0;

  virtual void enter_global_ctx(const lexer::token& name) = 0;
  virtual void leave_ctx() = 0;

  virtual std::unique_ptr<types_context> clone() const = 0;
};

class types_context_impl : public types_context
{
private:
  struct type_data
  {
    const sema_type& ty;
  };

public:
  void register_type(const lexer::token& name, const sema_type& ty) override;

  const sema_type* find(
    const std::vector<ast::name_with_coloncolon>& names) const override;

  const sema_type* find_in_current_scope(
    const lexer::token& name) const override;

  void enter_global_ctx(const lexer::token& name) override;
  void leave_ctx() override;

  std::unique_ptr<types_context> clone() const override;

private:
  qualified_entries_finder<type_data> m_types_finder;
};
}
