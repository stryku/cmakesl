#pragma once

#include "sema/function_lookup_result.hpp"
#include "sema/qualified_entries_finder.hpp"

#include <memory>

namespace cmsl {
namespace errors {
class errors_observer;
}

namespace sema {
class qualified_contexts_dumper;
class sema_function;
struct function_signature;

struct function_data
{
  const sema_function& fun;
};

class functions_context
{
public:
  virtual ~functions_context() = default;

  virtual void register_function(const lexer::token& name,
                                 const sema_function& ty, bool exported) = 0;

  virtual function_lookup_result_t find(
    const std::vector<ast::name_with_coloncolon>& names) const = 0;

  virtual const sema_function* find_in_current_scope(
    const function_signature& signature) const = 0;

  virtual void enter_global_ctx(const lexer::token& name, bool exported) = 0;
  virtual void leave_ctx() = 0;

  virtual std::unique_ptr<functions_context> clone() const = 0;
  virtual std::unique_ptr<functions_context> collect_exported_stuff()
    const = 0;

  virtual bool merge_imported_stuff(const functions_context& imported,
                                    errors::errors_observer& errs) = 0;

  virtual void dump(qualified_contexts_dumper& dumper) const = 0;
};

class functions_context_impl : public functions_context
{
public:
  void register_function(const lexer::token& name, const sema_function& fun,
                         bool exported) override;

  function_lookup_result_t find(
    const std::vector<ast::name_with_coloncolon>& names) const override;

  const sema_function* find_in_current_scope(
    const function_signature& signature) const override;

  void enter_global_ctx(const lexer::token& name, bool exported) override;
  void leave_ctx() override;

  std::unique_ptr<functions_context> clone() const override;
  std::unique_ptr<functions_context> collect_exported_stuff() const override;

  bool merge_imported_stuff(const functions_context& imported,
                            errors::errors_observer& errs) override;

  void dump(qualified_contexts_dumper& dumper) const override;

private:
  qualified_entries_finder<function_data> m_functions_finder;
};
}
}