#pragma once

#include "exec/scope_context.hpp"

#include <stack>

namespace cmsl::exec {
class execution_context
{
private:
  using instance_t = inst::instance;

public:
  class scope_leaving_guard
  {
  public:
    explicit scope_leaving_guard(std::stack<scope_context>& scopes)
      : m_scopes{ scopes }
    {
    }

    ~scope_leaving_guard()
    {
      if (!m_dismissed) {
        m_scopes.pop();
      }
    }

    void dismiss() { m_dismissed = true; }

  private:
    std::stack<scope_context>& m_scopes;
    bool m_dismissed{ false };
  };

  void add_variable(cmsl::string_view name, std::unique_ptr<instance_t> inst);
  instance_t* get_variable(cmsl::string_view name);
  instance_t* get_this();

  bool variable_exists(cmsl::string_view name) const;

  [[nodiscard]] scope_leaving_guard enter_scope();
  [[nodiscard]] scope_leaving_guard enter_member_function_scope(
    instance_t* class_instance);
  void leave_scope();

private:
  scope_context& current_scope();
  const scope_context& current_scope() const;

private:
  std::stack<scope_context> m_scopes;
};
}
