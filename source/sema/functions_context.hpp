#pragma once

#include "sema/function_lookup_result.hpp"
#include "sema/qualified_entries_finder.hpp"

#include <memory>

namespace cmsl::sema {

class sema_function;
struct function_signature;

class functions_context
{
public:
  virtual ~functions_context() = default;

  virtual void register_function(const lexer::token& name,
                                 const sema_function& ty) = 0;

  virtual function_lookup_result_t find(
    const std::vector<ast::name_with_coloncolon>& names) const = 0;

  virtual const sema_function* find_in_current_scope(
    const function_signature& signature) const = 0;

  virtual void enter_global_ctx(const lexer::token& name) = 0;
  virtual void leave_ctx() = 0;

  virtual std::unique_ptr<functions_context> clone() const = 0;
};

class functions_context_impl : public functions_context
{
private:
  struct function_data
  {
    const sema_function& fun;
  };

public:
  void register_function(const lexer::token& name,
                         const sema_function& fun) override;

  function_lookup_result_t find(
    const std::vector<ast::name_with_coloncolon>& names) const override;

  const sema_function* find_in_current_scope(
    const function_signature& signature) const override;

  void enter_global_ctx(const lexer::token& name) override;
  void leave_ctx() override;

  std::unique_ptr<functions_context> clone() const override;

private:
  qualified_entries_finder<function_data> m_functions_finder;
};

}
