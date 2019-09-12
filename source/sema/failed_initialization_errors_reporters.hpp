#pragma once

#include "sema/variable_initialization_issue.hpp"

namespace cmsl {

namespace errors {
class errors_observer;
struct error;
}

namespace sema {

class sema_function;
class expression_node;
class call_node;

namespace detail {
class designated_initializer_failed_initialization_errors_reporter
{
public:
  explicit designated_initializer_failed_initialization_errors_reporter(
    errors::errors_observer& errs);

  void operator()(
    const variable_initialization_issues::designated_different_types&);

  void operator()(const variable_initialization_issues::
                    designated_reference_init_from_temporary_value&);

  void operator()(
    const variable_initialization_issues::designated_member_not_found&);

  void operator()(const variable_initialization_issues::
                    designated_member_initialized_multiple_times&);

protected:
  virtual errors::error create_note_basics() const = 0;

protected:
  errors::errors_observer& m_errs;
};
}

class function_call_parameters_failed_initializations_errors_reporter
  : public detail::designated_initializer_failed_initialization_errors_reporter
{
public:
  explicit function_call_parameters_failed_initializations_errors_reporter(
    errors::errors_observer& errs, const sema_function& function,
    unsigned parameter_index, const expression_node& param_expression,
    const lexer::token& call_name);

  using designated_initializer_failed_initialization_errors_reporter::
  operator();

  void operator()(const variable_initialization_issues::different_types&);

  void operator()(const variable_initialization_issues::
                    reference_init_from_temporary_value&);

private:
  errors::error create_note_basics() const override;

private:
  const sema_function& m_function;
  unsigned m_param_index;
  const expression_node& m_param_expression;
  const lexer::token& m_call_name;
};

class function_return_value_failed_initialization_errors_reporter
  : public detail::designated_initializer_failed_initialization_errors_reporter
{
public:
  function_return_value_failed_initialization_errors_reporter(
    errors::errors_observer& errs, const sema_type& function_return_type,
    const lexer::token& return_kw);

  using designated_initializer_failed_initialization_errors_reporter::
  operator();

  void operator()(const variable_initialization_issues::different_types&);

  void operator()(const variable_initialization_issues::
                    reference_init_from_temporary_value&);

private:
  errors::error create_note_basics() const override;

private:
  const sema_type& m_function_return_type;
  const lexer::token& m_return_kw;
};

class variable_failed_initialization_errors_reporter
  : public detail::designated_initializer_failed_initialization_errors_reporter
{
public:
  variable_failed_initialization_errors_reporter(
    errors::errors_observer& errs, const lexer::token& equal_token);

  using designated_initializer_failed_initialization_errors_reporter::
  operator();

  void operator()(const variable_initialization_issues::different_types&);

  void operator()(const variable_initialization_issues::
                    reference_init_from_temporary_value&);

private:
  errors::error create_note_basics() const override;

private:
  const lexer::token& m_equal_token;
};
}
}
