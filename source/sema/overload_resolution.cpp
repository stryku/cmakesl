#include "sema/overload_resolution.hpp"
#include "common/overloaded.hpp"
#include "errors/error.hpp"
#include "errors/errors_observer.hpp"
#include "overload_resolution.hpp"
#include "sema/failed_initialization_errors_reporters.hpp"
#include "sema/sema_function.hpp"
#include "sema/sema_nodes.hpp"
#include "sema/variable_initialization_checker.hpp"

#include <algorithm>
#include <functional>

namespace cmsl::sema {
overload_resolution::overload_resolution(errors::errors_observer& errs,
                                         lexer::token call_token)
  : m_errs{ errs }
  , m_call_token{ call_token }
{
}

const sema_function* overload_resolution::choose(
  const function_lookup_result_t& functions,
  const std::vector<std::unique_ptr<expression_node>>& call_parameters) const
{
  if (functions.empty()) {
    const auto line_info =
      m_call_token.source().line(m_call_token.src_range().begin.line);
    const auto err = errors::error{ m_call_token.source().path(),
                                    line_info.line,
                                    line_info.start_pos,
                                    '\'' + std::string{ m_call_token.str() } +
                                      "\' function not found",
                                    errors::error_type::error,
                                    m_call_token.src_range() };
    m_errs.nofify_error(err);

    return nullptr;
  }

  const auto call_params_wrappers = to_vector_of_wrappers(call_parameters);

  for (const auto& scoped_functions : functions) {
    // Todo: is it even possible to have empty vector here?
    if (scoped_functions.empty()) {
      continue;
    }

    return choose_from_scope(scoped_functions, call_params_wrappers);
  }

  return nullptr;
}

const sema_function* overload_resolution::choose(
  const single_scope_function_lookup_result_t& functions,
  const std::vector<std::unique_ptr<expression_node>>& call_parameters) const
{
  const auto param_wrappers = to_vector_of_wrappers(call_parameters);
  return choose_from_scope(functions, param_wrappers);
}

const sema_function* overload_resolution::choose(
  const single_scope_function_lookup_result_t& functions,
  const expression_node& call_parameter) const
{
  const auto param_wrappers = to_vector_of_wrappers(call_parameter);
  return choose_from_scope(functions, param_wrappers);
}

const sema_function* overload_resolution::choose_from_scope(
  const single_scope_function_lookup_result_t& functions,
  const std::vector<std::reference_wrapper<const expression_node>>&
    call_parameters) const
{

  std::vector<function_match_result> results;

  for (const auto function : functions) {
    auto result = params_match(*function, call_parameters);
    if (std::holds_alternative<match_result::ok>(result)) {
      return function;
    }

    results.emplace_back(
      function_match_result{ *function, std::move(result) });
  }

  // At this point we know that there is no good function to call.
  raise_error(results, call_parameters);

  return nullptr;
}

overload_resolution::match_result_variant_t overload_resolution::params_match(
  const sema_function& function,
  const std::vector<std::reference_wrapper<const expression_node>>&
    call_parameters) const
{
  const auto& signature = function.signature();

  if (signature.params.size() != call_parameters.size()) {
    return match_result::wrong_parameters_count{
      static_cast<unsigned>(signature.params.size()),
      static_cast<unsigned>(call_parameters.size())
    };
  }

  //  std::vector<match_result::param_types_dont_match::mismatched_type_info>
  //    mismatched_types;

  std::vector<
    match_result::params_initialization_failed::param_initialization_info>
    initialization_failed_info;

  for (auto i = 0u; i < call_parameters.size(); ++i) {
    const auto& declared_param_type = signature.params[i].ty;
    const auto& param_expression_type = call_parameters[i].get().type();

    variable_initialization_checker checker;
    const auto init_issues =
      checker.check(declared_param_type, call_parameters[i].get());

    if (init_issues.empty()) {
      continue;
    }

    auto param_init_failed_info =
      match_result::params_initialization_failed::param_initialization_info{
        i, init_issues
      };

    initialization_failed_info.emplace_back(std::move(param_init_failed_info));
  }

  if (!initialization_failed_info.empty()) {
    return match_result::params_initialization_failed{ std::move(
      initialization_failed_info) };
  }

  return match_result::ok{};
}

overload_resolution::error_notes_reporter::error_notes_reporter(
  cmsl::errors::errors_observer& errs,
  const cmsl::sema::sema_function& function, const lexer::token& call_token,

  const std::vector<std::reference_wrapper<const expression_node>>&
    call_parameters)
  : m_errs{ errs }
  , m_function{ function }
  , m_call_token{ call_token }
  , m_call_parameters{ call_parameters }
{
}

void overload_resolution::error_notes_reporter::operator()(
  const overload_resolution::match_result::wrong_parameters_count& result)
  const
{
  auto err = create_note_basics();
  err.message += "Wrong parameters count passed. Expected " +
    std::to_string(result.expected) + ", got " + std::to_string(result.got);
  m_errs.nofify_error(err);
}

void overload_resolution::error_notes_reporter::operator()(
  const overload_resolution::match_result::params_initialization_failed&
    result) const
{

  for (const auto& info : result.info) {
    for (const auto& call_issue : info.issues) {
      const auto& param_expr = m_call_parameters[info.position].get();

      function_call_parameters_failed_initializations_errors_reporter reporter{
        m_errs, m_function, info.position, param_expr, m_call_token
      };

      std::visit(reporter, call_issue);
    }
  }

  /*

  auto err = create_note_basics();

  ////////////

  auto err = create_note_basics();
  const std::string parameters_str =
    result.info.size() > 1u ? "parameters" : "parameter";
  const auto position_str =
    result.info.size() > 1u ? "positions " : "position ";
  const auto suffix_str =
    result.info.size() > 1u ? " do not match" : " does not match";

  err.message += "Type of " + parameters_str + " at " + position_str;

  std::string separator{ "" };
  for (const auto& info : result.info) {
    err.message += separator + std::to_string(info.position);
    separator = ", ";
  }

  err.message += suffix_str;

  m_errs.nofify_error(err);

  // Todo: would be nice to create notes about expected and got types, with
  // expressions indication.
   */
}

errors::error overload_resolution::error_notes_reporter::create_note_basics()
  const
{
  errors::error err;
  const auto function_name = m_function.signature().name;
  const auto line_info =
    function_name.source().line(function_name.src_range().begin.line);
  err.line_snippet = line_info.line;
  err.line_start_pos = line_info.start_pos;
  err.source_path = function_name.source().path();
  err.range = function_name.src_range();
  err.type = errors::error_type::note;
  err.message = "candidate \'" + std::string{ function_name.str() } + "\'. ";
  return err;
}

void overload_resolution::raise_error(
  const std::vector<overload_resolution::function_match_result>& match_results,
  const std::vector<std::reference_wrapper<const expression_node>>&
    call_parameters) const
{
  {
    const auto line_info =
      m_call_token.source().line(m_call_token.src_range().begin.line);
    const auto err = errors::error{
      m_call_token.source().path(), line_info.line,
      line_info.start_pos,          "No matching function for call",
      errors::error_type::error,    m_call_token.src_range()
    };
    m_errs.nofify_error(err);
  }

  for (const auto& result : match_results) {
    const auto reporter =
      error_notes_reporter{ m_errs, result.function, m_call_token,
                            call_parameters };
    std::visit(reporter, result.params_match_result);
  }
}

std::vector<std::reference_wrapper<const expression_node>>
overload_resolution::to_vector_of_wrappers(
  const std::vector<std::unique_ptr<expression_node>>& params) const
{
  std::vector<std::reference_wrapper<const expression_node>> result;

  std::transform(
    std::cbegin(params), std::cend(params), std::back_inserter(result),
    [](const auto& param) -> std::reference_wrapper<const expression_node> {
      return *param;
    });

  return result;
}

std::vector<std::reference_wrapper<const expression_node>>
overload_resolution::to_vector_of_wrappers(const expression_node& param) const
{
  return { std::ref(param) };
}
}
