#include "sema/failed_initialization_errors_reporters.hpp"

#include "ast/return_node.hpp"
#include "errors/error.hpp"
#include "errors/errors_observer.hpp"
#include "sema/sema_nodes.hpp"

namespace cmsl::sema {

namespace detail {

designated_initializer_failed_initialization_errors_reporter::
  designated_initializer_failed_initialization_errors_reporter(
    errors::errors_observer& errs)
  : m_errs{ errs }
{
}

void designated_initializer_failed_initialization_errors_reporter::operator()(
  const variable_initialization_issues::designated_different_types& issue)
{
  // note: member 'foo' of type 'bar' can not be initialized by value of type
  // 'baz'

  auto err = create_note_basics();
  err.message = "member '" + std::string{ issue.member_name.str() } +
    "' of type '" + issue.member_type.name().to_string() +
    "' can not be initialized by a value of type '" +
    issue.init_type.name().to_string() + "'";
  m_errs.nofify_error(err);
}

void designated_initializer_failed_initialization_errors_reporter::operator()(
  const variable_initialization_issues::
    designated_reference_init_from_temporary_value& issue)
{
  // note: member 'foo' of a reference type, can not be initialized by a
  // temporary value

  auto err = create_note_basics();
  err.message = "member '" + std::string{ issue.member_name.str() } +
    "' of type '" + issue.member_type.name().to_string() +
    "' can not be initialized by a temporary value";
  m_errs.nofify_error(err);
}

void designated_initializer_failed_initialization_errors_reporter::operator()(
  const variable_initialization_issues::designated_member_not_found& issue)
{
  // note: no member named 'foo' in 'bar'

  auto err = create_note_basics();
  err.message = "no member named '" + std::string{ issue.member_name.str() } +
    "' in '" + issue.type.name().to_string() + "'";
  m_errs.nofify_error(err);
}

void designated_initializer_failed_initialization_errors_reporter::operator()(
  const variable_initialization_issues::
    designated_member_initialized_multiple_times& issue)
{
  // note: member 'foo' already initialized

  auto err = create_note_basics();
  err.message = "member '" + std::string{ issue.member_name.str() } +
    "' already initialized";
  m_errs.nofify_error(err);

  // note: initialized here
  err = create_note_basics();
  err.message = "initialized here";

  const auto line_info = issue.first_initialization.source().line(
    issue.first_initialization.src_range().begin.line);
  err.line_snippet = line_info.line;
  err.line_start_pos = line_info.start_pos;
  err.range = issue.first_initialization.src_range();

  m_errs.nofify_error(err);
}
}

function_call_parameters_failed_initializations_errors_reporter::
  function_call_parameters_failed_initializations_errors_reporter(
    errors::errors_observer& errs, const sema_function& function,
    unsigned parameter_index, const expression_node& param_expression,
    const lexer::token& call_name)
  : designated_initializer_failed_initialization_errors_reporter{ errs }
  , m_function{ function }
  , m_param_index{ parameter_index }
  , m_param_expression{ param_expression }
  , m_call_name{ call_name }
{
}

void function_call_parameters_failed_initializations_errors_reporter::
operator()(const variable_initialization_issues::different_types& issue)
{
  // note: type of parameter at position 0 does not match. Expected 'foo', got
  // 'bar'
  auto err = create_note_basics();

  err.message = "type of parameter at position " +
    std::to_string(m_param_index) + " does not match. Expected '" +
    m_function.signature().params[m_param_index].ty.name().to_string() +
    "', got '" + m_param_expression.type().name().to_string() + '\'';

  m_errs.nofify_error(err);
}

void function_call_parameters_failed_initializations_errors_reporter::
operator()(
  const variable_initialization_issues::reference_init_from_temporary_value&)
{
  // note: parameter at position 0, of a reference type, initialized by a
  // temporary value
  auto err = create_note_basics();
  err.message = "parameter at position " + std::to_string(m_param_index) +
    ", of a reference type, initialized by a temporary value";
  m_errs.nofify_error(err);
}

errors::error function_call_parameters_failed_initializations_errors_reporter::
  create_note_basics() const
{
  errors::error err;
  const auto line_info =
    m_call_name.source().line(m_call_name.src_range().begin.line);
  err.line_snippet = line_info.line;
  err.line_start_pos = line_info.start_pos;
  err.source_path = m_call_name.source().path();
  err.range = m_param_expression.ast_node().src_range();
  err.type = errors::error_type::note;
  return err;
}

function_return_value_failed_initialization_errors_reporter::
  function_return_value_failed_initialization_errors_reporter(
    errors::errors_observer& errs, const sema_type& function_return_type,
    const lexer::token& return_kw)
  : designated_initializer_failed_initialization_errors_reporter{ errs }
  , m_function_return_type{ function_return_type }
  , m_return_kw{ return_kw }
{
}

void function_return_value_failed_initialization_errors_reporter::operator()(
  const variable_initialization_issues::different_types& issue)
{
  // note: return type and value type do not match. expected 'foo', got 'bar'
  auto err = create_note_basics();
  err.message = "return type and value type do not match. expected '" +
    issue.expected.name().to_string() + "', got '" +
    issue.got.name().to_string() + "'";

  m_errs.nofify_error(err);
}

void function_return_value_failed_initialization_errors_reporter::operator()(
  const variable_initialization_issues::reference_init_from_temporary_value&
    issue)
{
  // note: return value of a reference type can not be initialized by a
  // temporary value
  auto err = create_note_basics();
  err.message = "return value of a reference type can not be initialized by a "
                "temporary value";
  m_errs.nofify_error(err);
}

errors::error function_return_value_failed_initialization_errors_reporter::
  create_note_basics() const
{
  errors::error err;
  const auto line_info =
    m_return_kw.source().line(m_return_kw.src_range().begin.line);
  err.line_snippet = line_info.line;
  err.line_start_pos = line_info.start_pos;
  err.source_path = m_return_kw.source().path();
  err.range = m_return_kw.src_range();
  err.type = errors::error_type::error;
  return err;
}

variable_failed_initialization_errors_reporter::
  variable_failed_initialization_errors_reporter(
    errors::errors_observer& errs, const lexer::token& equal_token)
  : designated_initializer_failed_initialization_errors_reporter{ errs }
  , m_equal_token{ equal_token }
{
}

void variable_failed_initialization_errors_reporter::operator()(
  const variable_initialization_issues::different_types&)
{
  auto err = create_note_basics();
  err.message = "Initialization and declared variable type does not match";
  m_errs.nofify_error(err);
}

void variable_failed_initialization_errors_reporter::operator()(
  const variable_initialization_issues::reference_init_from_temporary_value&)
{
  auto err = create_note_basics();
  err.message =
    "Reference variable can not be initialized with a temporary value";
  m_errs.nofify_error(err);
}

errors::error
variable_failed_initialization_errors_reporter::create_note_basics() const
{
  errors::error err;
  const auto line_info =
    m_equal_token.source().line(m_equal_token.src_range().begin.line);
  err.line_snippet = line_info.line;
  err.line_start_pos = line_info.start_pos;
  err.source_path = m_equal_token.source().path();
  err.range = m_equal_token.src_range();
  err.type = errors::error_type::error;
  return err;
}
}
