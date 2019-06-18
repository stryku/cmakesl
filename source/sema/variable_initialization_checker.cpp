#include "sema/variable_initialization_checker.hpp"

#include "sema/sema_nodes.hpp"
#include "sema/sema_type.hpp"
#include "variable_initialization_checker.hpp"

namespace cmsl::sema {
variable_initialization_checker::check_result
variable_initialization_checker::check(
  const sema_type& variable_type,
  const expression_node& initialization_expression) const
{
  const auto& expression_type = initialization_expression.type();

  const auto expr_t = expression_type.name().to_string();
  const auto var_t = variable_type.name().to_string();

  if (variable_type == expression_type) {
    return check_result::can_init;
  }

  if (effective_type(variable_type) != effective_type(expression_type)) {
    return check_result::different_types;
  }

  if (variable_type.is_reference()) {
    return check_initialization_of_reference_type(variable_type,
                                                  initialization_expression);
  } else {
    return check_initialization(variable_type, initialization_expression);
  }
}

variable_initialization_checker::check_result
variable_initialization_checker::check_initialization_of_reference_type(
  const sema_type& variable_type,
  const expression_node& initialization_expression) const
{
  const auto& expression_type = initialization_expression.type();

  const auto& variable_referenced_type = variable_type.referenced_type();
  if (variable_referenced_type == expression_type) {
    if (initialization_expression.produces_temporary_value()) {
      return check_result::reference_init_from_temporary_value;
    }

    return check_result::can_init;
  }

  return check_initialization(variable_referenced_type,
                              initialization_expression);
}

variable_initialization_checker::check_result
variable_initialization_checker::check_initialization(
  const sema_type& variable_type,
  const expression_node& initialization_expression) const
{
  const auto& expression_type = initialization_expression.type();

  if (!expression_type.is_reference()) {
    return check_result::different_types;
  }

  const auto& expression_referenced_type = expression_type.referenced_type();
  if (variable_type != expression_referenced_type) {
    return check_result::different_types;
  }

  return check_result::can_init;
}

const sema_type& variable_initialization_checker::effective_type(
  const sema_type& type) const
{
  return type.is_reference() ? type.referenced_type() : type;
}
}