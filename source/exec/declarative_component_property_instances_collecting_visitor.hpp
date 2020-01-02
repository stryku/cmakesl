#include "cmake_facade.hpp"
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
#include "sema/sema_type.hpp"

#include <unordered_map>

namespace cmsl::exec {
class declarative_component_property_instances_collecting_visitor
  : public decl_sema::sema_node_visitor
{
public:
  explicit declarative_component_property_instances_collecting_visitor(
    facade::cmake_facade& facade,
    const sema::builtin_types_accessor& builtin_types,
    inst::instances_holder_interface& instances)
    : m_facade{ facade }
    , m_builtin_types{ builtin_types }
    , m_instances{ instances }
  {
  }

  void visit(const decl_sema::bool_value_node& node) override
  {
    auto created = m_instances.create(node.value());
    m_result = m_instances.gather_ownership(created);
  }

  void visit(const decl_sema::component_node& node) override
  {
    for (const auto& child : node.nodes()) {
      const auto property_node =
        dynamic_cast<const decl_sema::property_node*>(child.get());
      CMSL_ASSERT_MSG(property_node, "Expected property node");

      auto value = evaluate_child(property_node->value());
      m_collected_properties[&property_node->property_access()] =
        std::move(value);
    }
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

  void visit(const decl_sema::property_node& node) override {}
  void visit(const decl_sema::property_access_node& node) override {}

  void visit(const decl_sema::string_value_node& node) override
  {
    auto created = m_instances.create(std::string{ node.value() });
    m_result = m_instances.gather_ownership(created);
  }

private:
  declarative_component_property_instances_collecting_visitor clone()
  {
    return declarative_component_property_instances_collecting_visitor{
      m_facade,
      m_builtin_types,
      m_instances,
    };
  }

  std::unique_ptr<exec::inst::instance> evaluate_child(
    const decl_sema::expression_node& node)
  {
    auto cloned = clone();
    node.visit(cloned);
    return std::move(cloned.m_result);
  }

public:
  std::unordered_map<const decl_sema::property_access_node*,
                     std::unique_ptr<inst::instance>>
    m_collected_properties;

private:
  std::unique_ptr<exec::inst::instance> m_result;

private:
  facade::cmake_facade& m_facade;
  const sema::builtin_types_accessor& m_builtin_types;
  inst::instances_holder_interface& m_instances;
};
}
