#pragma once

#include "sema/qualified_entries_finder.hpp"

#include <functional>
#include <memory>

namespace cmsl {
namespace errors {
class errors_observer;
}

namespace sema {
class sema_type;
class qualified_contexts_dumper;

struct enum_value_info
{
  std::reference_wrapper<const sema_type> type;
  unsigned value;
  unsigned index;
};

class enum_values_context
{
protected:
  using token_t = lexer::token;
  using qualified_names_t = std::vector<ast::name_with_coloncolon>;

public:
  virtual ~enum_values_context() = default;

  virtual void register_identifier(token_t name, enum_value_info info,
                                   bool exported) = 0;
  virtual std::optional<enum_value_info> info_of(
    const qualified_names_t& names) const = 0;

  virtual void enter_global_ctx(token_t name, bool exported) = 0;
  virtual void leave_ctx() = 0;

  virtual std::unique_ptr<enum_values_context> clone() const = 0;
  virtual std::unique_ptr<enum_values_context> collect_exported_stuff()
    const = 0;

  virtual bool merge_imported_stuff(const enum_values_context& imported,
                                    errors::errors_observer& errs) = 0;

  virtual void dump(qualified_contexts_dumper& dumper) const = 0;
};

class enum_values_context_impl : public enum_values_context
{
public:
  void register_identifier(lexer::token declaration_token,
                           enum_value_info info, bool exported) override;

  std::optional<enum_value_info> info_of(
    const qualified_names_t& names) const override;

  void enter_global_ctx(token_t name, bool exported) override;
  void leave_ctx() override;

  std::unique_ptr<enum_values_context> clone() const override;
  std::unique_ptr<enum_values_context> collect_exported_stuff() const override;

  bool merge_imported_stuff(const enum_values_context& imported,
                            errors::errors_observer& errs) override;

  void dump(qualified_contexts_dumper& dumper) const override;

private:
  qualified_entries_finder<enum_value_info> m_finder;
};
}
}