#pragma once

#include "sema/sema_context.hpp"

namespace cmsl::sema {
class sema_context_impl : public sema_context
{
public:
  explicit sema_context_impl(
    std::string name, const sema_context* parent = nullptr,
    context_type context_type = context_type::namespace_);

  void add_function(const sema_function& function) override;
  void add_type(const sema_type& type) override;
  const sema_type* find_type(
    const ast::type_representation& name) const override;
  const sema_type* find_referenced_type(
    const ast::type_representation& name) const override;
  const sema_type* find_reference_for(const sema_type& type) const override;
  const sema_type* find_type_in_this_scope(
    const ast::type_representation& name) const override;
  function_lookup_result_t find_function(
    const lexer::token& name) const override;
  single_scope_function_lookup_result_t find_function_in_this_scope(
    const lexer::token& name) const override;
  context_type type() const override;
  std::vector<std::reference_wrapper<const sema_type>> types() const override;
  std::string fully_qualified_name() const override;

  const sema_context* find_ctx_for_type_lookup(
    const ast::type_representation& name) const override;

  const sema_context* parent() const override { return m_parent; }

private:
  template <typename Predicate>
  const sema_type* find_type_in_this_scope_with_predicate(
    Predicate&& pred) const;

  const sema_context* find_ctx_containing_type(
    const std::string& name) const override;

  const sema_context* find_root() const;

private:
  const sema_context* m_parent;
  std::vector<const sema_function*> m_functions;
  std::vector<const sema_type*> m_types;
  context_type m_context_type;
  std::string m_name;
};
}
