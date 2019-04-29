#pragma once

#include "lexer/token.hpp"
#include "sema/function_lookup_result.hpp"

#include <memory>
#include <variant>

namespace cmsl {
namespace errors {
struct error;
class errors_observer;
}

namespace sema {
class sema_type;
class expression_node;

class overload_resolution
{
public:
  // Todo: consider accepting source range instead of token
  explicit overload_resolution(errors::errors_observer& errs,
                               lexer::token call_token);

  // Todo: use small vector
  const sema_function* choose(
    const function_lookup_result_t& functions,
    const std::vector<std::unique_ptr<expression_node>>& call_parameters)
    const;
  const sema_function* choose(
    const single_scope_function_lookup_result_t& functions,
    const std::vector<std::unique_ptr<expression_node>>& call_parameters)
    const;
  const sema_function* choose(
    const single_scope_function_lookup_result_t& functions,
    const expression_node& call_parameter) const;

private:
  struct match_result
  {
    struct ok
    {
    };

    struct wrong_parameters_count
    {
      unsigned expected{ 0u };
      unsigned got{ 0u };
    };

    struct param_types_dont_match
    {
      struct mismatched_type_info
      {
        unsigned position;
        const sema_type& expected;
        const sema_type& got;
      };

      // Todo: use small vector
      std::vector<mismatched_type_info> info;
    };
  };

  using match_result_variant_t =
    std::variant<match_result::ok, match_result::wrong_parameters_count,
                 match_result::param_types_dont_match>;

  struct function_match_result
  {
    const sema_function& function;
    match_result_variant_t params_match_result;
  };

  class error_notes_reporter
  {
  public:
    explicit error_notes_reporter(errors::errors_observer& errs,
                                  const sema_function& function);

    void operator()(const match_result::ok&) const {};
    void operator()(const match_result::wrong_parameters_count& result) const;
    void operator()(const match_result::param_types_dont_match& result) const;

  private:
    errors::error create_note_basics() const;

  private:
    errors::errors_observer& m_errs;
    const sema_function& m_function;
  };

  const sema_function* choose_from_scope(
    const single_scope_function_lookup_result_t& functions,
    const std::vector<std::reference_wrapper<const expression_node>>&
      call_parameters) const;

  match_result_variant_t params_match(
    const sema_function& function,
    const std::vector<std::reference_wrapper<const expression_node>>&
      call_parameters) const;

  std::vector<std::reference_wrapper<const expression_node>>
  to_vector_of_wrappers(
    const std::vector<std::unique_ptr<expression_node>>& params) const;
  std::vector<std::reference_wrapper<const expression_node>>
  to_vector_of_wrappers(const expression_node& param) const;

  void raise_error(
    const std::vector<function_match_result>& match_results) const;

private:
  errors::errors_observer& m_errs;
  lexer::token m_call_token;
};
}
}
