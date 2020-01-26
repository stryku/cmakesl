#include "exec/declarative_component_property_instances_collecting_visitor.hpp"
#include "common/algorithm.hpp"
#include "errors/error.hpp"
#include "errors/errors_observer.hpp"

namespace cmsl::exec {
declarative_component_property_instances_collecting_visitor::
  declarative_component_property_instances_collecting_visitor(
    facade::cmake_facade& facade,
    const sema::builtin_types_accessor& builtin_types,
    inst::instances_holder_interface& instances,
    sema::generic_type_creation_utils generic_types,
    inst::instance& component_instance,
    errors::errors_observer& errors_observer)
  : m_facade{ facade }
  , m_builtin_types{ builtin_types }
  , m_instances{ instances }
  , m_generic_types{ generic_types }
  , m_component_instance{ component_instance }
  , m_errs{ errors_observer }
{
}

void declarative_component_property_instances_collecting_visitor::visit(
  const decl_sema::bool_value_node& node)
{
  auto created = m_instances.create(node.value());
  m_result = m_instances.gather_ownership(created);
}

void declarative_component_property_instances_collecting_visitor::visit(
  const decl_sema::component_node& node)
{
  if (const auto component_declaration = node.component_declaration()) {
    component_declaration->visit(*this);
  }

  for (const auto& child : node.nodes()) {
    child->visit(*this);
  }
}

void declarative_component_property_instances_collecting_visitor::visit(
  const decl_sema::component_declaration_node& node)
{
  if (const auto derived_component_declaration =
        node.derived_component_declaration()) {
    derived_component_declaration->visit(*this);
  }

  for (const auto& child : node.nodes()) {
    child->visit(*this);
  }
}

void declarative_component_property_instances_collecting_visitor::visit(
  const decl_sema::translation_unit_node& node)
{
  CMSL_UNREACHABLE("Should not be here");
}

void declarative_component_property_instances_collecting_visitor::visit(
  const decl_sema::double_value_node& node)
{
  auto created = m_instances.create(node.value());
  m_result = m_instances.gather_ownership(created);
}

void declarative_component_property_instances_collecting_visitor::visit(
  const decl_sema::int_value_node& node)
{
  auto created = m_instances.create(node.value());
  m_result = m_instances.gather_ownership(created);
}

void declarative_component_property_instances_collecting_visitor::visit(
  const decl_sema::list_node& node)
{
  auto list_instance = m_instances.create(node.type());
  auto accessor = list_instance->value_accessor();
  auto& list = accessor.access().get_list_ref();

  for (const auto& value_expression : node.values()) {
    auto evaluated = evaluate_child(*value_expression);
    list.push_back(std::move(evaluated));
  }

  m_result = m_instances.gather_ownership(list_instance);
}

void declarative_component_property_instances_collecting_visitor::visit(
  const decl_sema::property_node& node)
{
  auto value = evaluate_child(node.value());
  auto& property_instance =
    access_property_instance(m_component_instance, node.property_access());
  property_instance.assign(std::move(value));
}

void declarative_component_property_instances_collecting_visitor::visit(
  const decl_sema::property_append_node& node)
{
  auto value = evaluate_child(node.value());

  auto& property_instance =
    access_property_instance(m_component_instance, node.property_access());
  auto property_instance_accessor = property_instance.value_accessor();
  auto& list = property_instance_accessor.access().get_list_ref();

  if (value->value_cref().which() ==
      inst::instance_value_variant::which_t::list) {
    list.push_back(value->value_cref().get_list_cref());
  } else {
    CMSL_ASSERT(value->value_cref().which() ==
                inst::instance_value_variant::which_t::string);
    list.push_back(std::move(value));
  }
}

void declarative_component_property_instances_collecting_visitor::visit(
  const decl_sema::property_access_node& node)
{
  CMSL_UNREACHABLE("Should not be here");
}

void declarative_component_property_instances_collecting_visitor::visit(
  const decl_sema::cmake_variable_access_node& node)
{
  const auto& variable_name = node.variable_name();

  const auto cmake_variable =
    m_facade.get_old_style_variable(std::string{ variable_name.str() });
  if (cmake_variable) {
    m_result =
      convert_cmake_variable_to_instance_of_type(*cmake_variable, node.type());

    return;
  }

  // Variable doesn't exist. Raise a warning.
  const auto line_info =
    variable_name.source().line(variable_name.src_range().begin.line);
  errors::error warning{ variable_name.source().path(),
                         line_info.line,
                         line_info.start_pos,
                         "getting not existing variable '" +
                           std::string{ variable_name.str() } + "'",
                         errors::error_type ::warning,
                         variable_name.src_range() };

  m_errs.notify_error(warning);

  m_result =
    convert_cmake_variable_to_instance_of_type(std::string{}, node.type());
}

void declarative_component_property_instances_collecting_visitor::visit(
  const decl_sema::string_value_node& node)
{
  auto created = m_instances.create(std::string{ node.value() });
  m_result = m_instances.gather_ownership(created);
}

void declarative_component_property_instances_collecting_visitor::visit(
  const decl_sema::import_node& node)
{
  CMSL_UNREACHABLE("Should not be here.");
}

std::unique_ptr<inst::instance>
declarative_component_property_instances_collecting_visitor::
  convert_cmake_variable_to_instance_of_type(
    const std::string& cmake_variable, const sema::sema_type& desired_type)
{
  inst::instance* created{ nullptr };

  if (desired_type == m_builtin_types.bool_) {
    const auto true_values = { "on", "true", "1" };
    const auto lower = to_lower(cmake_variable);
    const auto value = contains(true_values, lower);
    created = m_instances.create(value);
  }

  else if (desired_type == m_builtin_types.int_) {
    const int_t value = std::stol(cmake_variable);
    created = m_instances.create(value);
  }

  else if (desired_type == m_builtin_types.double_) {
    const int_t value = std::stod(cmake_variable);
    created = m_instances.create(value);
  }

  else if (desired_type == m_builtin_types.string) {
    created = m_instances.create(cmake_variable);
  } else {
    std::vector<std::string> values = split(cmake_variable, ';');

    const auto& list_type = m_generic_types.list_of_strings();

    created = m_instances.create(list_type);
    auto accessor = created->value_accessor();
    auto& list = accessor.access().get_list_ref();

    for (auto& value : values) {
      const auto value_instance = m_instances.create(std::move(value));
      list.push_back(m_instances.gather_ownership(value_instance));
    }
  }

  CMSL_ASSERT(created);

  return m_instances.gather_ownership(created);
}

declarative_component_property_instances_collecting_visitor
declarative_component_property_instances_collecting_visitor::clone()
{
  return declarative_component_property_instances_collecting_visitor{
    m_facade,        m_builtin_types,      m_instances,
    m_generic_types, m_component_instance, m_errs
  };
}

std::unique_ptr<exec::inst::instance>
declarative_component_property_instances_collecting_visitor::evaluate_child(
  const decl_sema::expression_node& node)
{
  auto cloned = clone();
  node.visit(cloned);
  return std::move(cloned.m_result);
}

inst::instance& declarative_component_property_instances_collecting_visitor::
  access_property_instance(inst::instance& component_instance,
                           const decl_sema::property_access_node& node)
{
  auto current_instance = &component_instance;
  for (const auto& property_info : node.properties_access()) {
    const auto found_member =
      current_instance->find_member(property_info.index);
    CMSL_ASSERT(found_member);

    current_instance = found_member;
  }

  return *current_instance;
}
}
