#include "sema/variable_initialization_checker.hpp"

#include "common/algorithm.hpp"
#include "common/overloaded.hpp"
#include "sema/sema_nodes.hpp"
#include "sema/sema_type.hpp"

#include <unordered_set>

namespace cmsl::sema {

using namespace cmsl::sema::variable_initialization_issues;

std::vector<variable_initialization_checker::issue_t>
variable_initialization_checker::check(
  const sema_type& variable_type,
  const expression_node& initialization_expression) const
{
  const auto& expression_type = initialization_expression.type();

  if (variable_type == expression_type) {
    return {};
  }

  if (expression_type.derives_from(variable_type)) {
    return {};
  }

  std::vector<issue_t> issues;

  if (expression_type.is_designated_initializer()) {
    const auto& designated_node =
      dynamic_cast<const designated_initializers_node&>(
        initialization_expression);
    return check_designated_initializers(variable_type, designated_node);
  }

  if (variable_type.decayed() != expression_type.decayed()) {
    issues.emplace_back(different_types{ variable_type, expression_type });
    return issues;
  }

  if (variable_type.is_reference()) {
    const auto issue = check_initialization_of_reference_type(
      variable_type, initialization_expression);
    if (issue) {
      issues.emplace_back(*issue);
    }
  } else {
    const auto issue =
      check_initialization(variable_type, initialization_expression);
    if (issue) {
      issues.emplace_back(*issue);
    }
  }

  return issues;
}

std::optional<variable_initialization_checker::issue_t>
variable_initialization_checker::check_initialization_of_reference_type(
  const sema_type& variable_type,
  const expression_node& initialization_expression) const
{
  const auto& expression_type = initialization_expression.type();

  const auto& variable_referenced_type = variable_type.referenced_type();
  if (variable_referenced_type == expression_type) {
    if (initialization_expression.produces_temporary_value()) {
      return reference_init_from_temporary_value{};
    }

    return std::nullopt;
  }

  return check_initialization(variable_referenced_type,
                              initialization_expression);
}

std::optional<variable_initialization_checker::issue_t>
variable_initialization_checker::check_initialization(
  const sema_type& variable_type,
  const expression_node& initialization_expression) const
{
  const auto& expression_type = initialization_expression.type();

  if (!expression_type.is_reference()) {
    return different_types{ variable_type, expression_type };
  }

  const auto& expression_referenced_type = expression_type.referenced_type();
  if (variable_type != expression_referenced_type) {
    return different_types{ variable_type, expression_type };
  }

  return std::nullopt;
}

std::vector<variable_initialization_checker::issue_t>
variable_initialization_checker::check_designated_initializers(
  const sema_type& variable_type,
  const designated_initializers_node& designated_initializers) const
{
  std::vector<issue_t> issues;

  if (variable_type.is_reference()) {
    issues.emplace_back(reference_init_from_temporary_value{});
  }

  std::unordered_set<token_t> already_initialized;

  const auto& inits = designated_initializers.initializers();
  for (auto i = 0u; i < inits.size(); ++i) {
    const auto& init = inits[i];
    if (contains(already_initialized, init.name)) {
      const auto issue = designated_member_initialized_multiple_times{
        .member_name = init.name,
        .first_initialization = *already_initialized.find(init.name)
      };
      issues.emplace_back(issue);
      continue;
    }

    already_initialized.insert(init.name);

    const auto member_info = variable_type.find_member(init.name.str());
    if (!member_info) {
      const auto issue = designated_member_not_found{ .member_name = init.name,
                                                      .type = variable_type };
      issues.emplace_back(issue);
      continue;
    }

    variable_initialization_checker checker;
    const auto member_init_result = checker.check(member_info->ty, *init.init);

    for (const auto& issue : member_init_result) {
      std::visit(
        overloaded{
          [&](const different_types&) {
            const auto iss =
              designated_different_types{ .member_name = init.name,
                                          .member_type = member_info->ty,
                                          .init_type = init.init->type() };
            issues.emplace_back(iss);
          },
          [&](const reference_init_from_temporary_value&) {
            const auto iss = designated_reference_init_from_temporary_value{
              init.name, member_info->ty
            };
            issues.emplace_back(iss);
          },
          [&](const auto& iss) { issues.emplace_back(iss); } },
        issue);
    }
  }
  return issues;
}
}
