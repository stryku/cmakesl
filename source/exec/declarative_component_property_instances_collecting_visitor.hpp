#include "cmake_facade.hpp"
#include "common/algorithm.hpp"
#include "common/assert.hpp"
#include "decl_sema/component_creation_sema_function.hpp"
#include "decl_sema/sema_node_visitor.hpp"
#include "decl_sema/sema_nodes.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instance_factory.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/instance/list_value_utils.hpp"
#include "sema/cmake_namespace_types_accessor.hpp"
#include "sema/factories.hpp"
#include "sema/factories_provider.hpp"
#include "sema/generic_type_creation_utils.hpp"
#include "sema/sema_type.hpp"

#include <unordered_map>

namespace cmsl::exec {
class declarative_component_property_instances_collecting_visitor
  : public decl_sema::sema_node_visitor
{
public:
  using collected_properties_t =
    std::unordered_map<const decl_sema::property_access_node*,
                       std::unique_ptr<inst::instance>>;

  explicit declarative_component_property_instances_collecting_visitor(
    facade::cmake_facade& facade,
    const sema::builtin_types_accessor& builtin_types,
    inst::instances_holder_interface& instances,
    sema::generic_type_creation_utils generic_types,
    inst::instance& component_instance)
    : m_facade{ facade }
    , m_builtin_types{ builtin_types }
    , m_instances{ instances }
    , m_generic_types{ generic_types }
    , m_component_instance{ component_instance }
  {
  }

  void visit(const decl_sema::bool_value_node& node) override
  {
    auto created = m_instances.create(node.value());
    m_result = m_instances.gather_ownership(created);
  }

  void visit(const decl_sema::component_node& node) override
  {
    if (const auto component_declaration = node.component_declaration()) {
      component_declaration->visit(*this);
    }

    for (const auto& child : node.nodes()) {
      child->visit(*this);
    }
  }

  void visit(const decl_sema::component_declaration_node& node) override
  {
    if (const auto derived_component_declaration =
          node.derived_component_declaration()) {
      derived_component_declaration->visit(*this);
    }

    for (const auto& child : node.nodes()) {
      child->visit(*this);
    }
  }

  void visit(const decl_sema::translation_unit_node& node) override
  {
    CMSL_UNREACHABLE("Should not be here");
  }

  void visit(const decl_sema::double_value_node& node) override
  {
    auto created = m_instances.create(node.value());
    m_result = m_instances.gather_ownership(created);
  }

  void visit(const decl_sema::int_value_node& node) override
  {
    auto created = m_instances.create(node.value());
    m_result = m_instances.gather_ownership(created);
  }

  void visit(const decl_sema::list_node& node) override
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

  void visit(const decl_sema::property_node& node) override
  {
    auto value = evaluate_child(node.value());
    auto& property_instance =
      access_property_instance(m_component_instance, node.property_access());
    property_instance.assign(std::move(value));
  }

  void visit(const decl_sema::property_append_node& node) override
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

  void visit(const decl_sema::property_access_node& node) override {}

  void visit(const decl_sema::cmake_variable_access_node& node) override
  {
    const auto cmake_variable = m_facade.get_old_style_variable(
      std::string{ node.variable_name().str() });
    m_result =
      convert_cmake_variable_to_instance_of_type(cmake_variable, node.type());
  }

  void visit(const decl_sema::string_value_node& node) override
  {
    auto created = m_instances.create(std::string{ node.value() });
    m_result = m_instances.gather_ownership(created);
  }

private:
  std::unique_ptr<inst::instance> convert_cmake_variable_to_instance_of_type(
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

  declarative_component_property_instances_collecting_visitor clone()
  {
    return declarative_component_property_instances_collecting_visitor{
      m_facade, m_builtin_types, m_instances, m_generic_types,
      m_component_instance
    };
  }

  std::unique_ptr<exec::inst::instance> evaluate_child(
    const decl_sema::expression_node& node)
  {
    auto cloned = clone();
    node.visit(cloned);
    return std::move(cloned.m_result);
  }

  inst::instance& access_property_instance(
    inst::instance& component_instance,
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

private:
  facade::cmake_facade& m_facade;
  const sema::builtin_types_accessor& m_builtin_types;
  inst::instances_holder_interface& m_instances;
  sema::generic_type_creation_utils m_generic_types;
  inst::instance& m_component_instance;

  std::unique_ptr<exec::inst::instance> m_result;
};
}
