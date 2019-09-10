#pragma once

#include "exec/expression_evaluation_visitor.hpp"
#include "exec/function_caller.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/module_sema_tree_provider.hpp"
#include "exec/module_static_variables_initializer.hpp"
#include "sema/sema_node_visitor.hpp"
#include "sema/sema_nodes.hpp"

#include <unordered_map>

namespace cmsl::exec {

class static_variables_initializer final
  : public sema::empty_sema_node_visitor
  , public identifiers_context
{
public:
  explicit static_variables_initializer(
    function_caller& caller, const sema::builtin_types_accessor& builtin_types,
    facade::cmake_facade& cmake_facade,
    module_static_variables_initializer& module_variables_initializer)
    : m_function_caller{ caller }
    , m_builtin_types{ builtin_types }
    , m_cmake_facade{ cmake_facade }
    , m_module_variables_initializer{ module_variables_initializer }
  {
  }

  ~static_variables_initializer() = default;

  inst::instance* lookup_identifier(unsigned index) override
  {
    const auto found = m_instances.find(index);
    return found == std::cend(m_instances) ? nullptr : found->second.get();
  }

  inst::instance* get_class_instance() override { return nullptr; }

  void visit(const sema::variable_declaration_node& node) override
  {
    inst::instances_holder instances{ m_builtin_types };
    expression_evaluation_context ctx{ m_function_caller, instances, *this,
                                       m_cmake_facade };
    ctx.expected_types.push(node.type());
    std::unique_ptr<inst::instance> created_instance;

    if (auto initialization = node.initialization()) {
      expression_evaluation_visitor visitor{ ctx };
      initialization->visit(visitor);

      if (instances.owns(visitor.result)) {
        created_instance = instances.gather_ownership(visitor.result);
      } else {
        created_instance = visitor.result->copy();
      }
    } else {
      auto variable_instance_ptr = instances.create(node.type());
      created_instance = instances.gather_ownership(variable_instance_ptr);
    }

    m_instances.emplace(node.index(), std::move(created_instance));
  }

  void visit(const sema::class_node& node) override
  {
    // Todo: implement initialization of static members, when (and if) static
    // members are implemented.
  }

  void visit(const sema::translation_unit_node& node) override
  {
    for (const auto& child : node.nodes()) {
      child->visit(*this);
    }
  }

  void visit(const sema::namespace_node& node) override
  {
    for (const auto& child : node.nodes()) {
      child->visit(*this);
    }
  }

  void visit(const sema::import_node& node) override
  {
    m_module_variables_initializer.initialize(node.pretty_file_path());
  }

  std::unordered_map<unsigned, std::unique_ptr<inst::instance>>
  gather_instances()
  {
    return std::move(m_instances);
  }

private:
  function_caller& m_function_caller;
  facade::cmake_facade& m_cmake_facade;
  const sema::builtin_types_accessor& m_builtin_types;
  module_static_variables_initializer& m_module_variables_initializer;
  std::unordered_map<unsigned, std::unique_ptr<inst::instance>> m_instances;
};
}
