#pragma once

#include "lexer/token.hpp"
#include "sema/variable_initialization_issue.hpp"

#include <variant>

namespace cmsl::sema {
class sema_type;
class expression_node;
class designated_initializers_node;

class variable_initialization_checker
{
public:
  using issue_t = variable_initialization_issue_t;

  // Returns a vector of isses. If the vector is empty, variable can be
  // initialized by a given expression.
  std::vector<issue_t> check(
    const sema_type& variable_type,
    const expression_node& initialization_expression) const;

private:
  // For reference types returns the referenced type.
  const sema_type& effective_type(const sema_type& type) const;

  std::optional<issue_t> check_initialization_of_reference_type(
    const sema_type& variable_type,
    const expression_node& initialization_expression) const;
  std::optional<issue_t> check_initialization(
    const sema_type& variable_type,
    const expression_node& initialization_expression) const;

  std::vector<issue_t> check_designated_initializers(
    const sema_type& variable_type,
    const designated_initializers_node& designated_initializers) const;
};
}
