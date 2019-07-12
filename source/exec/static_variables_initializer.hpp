#pragma once

#include "exec/expression_evaluation_visitor.hpp"
#include "exec/function_caller.hpp"
#include "exec/instance/instances_holder.hpp"
#include "sema/sema_node_visitor.hpp"
#include "sema/sema_nodes.hpp"

#include <unordered_map>

namespace cmsl::exec {

class static_variables_initializer final
  : public sema::empty_sema_node_visitor
  , public identifiers_context
{
public:
  explicit static_variables_initializer(function_caller& caller,
                                        const sema::sema_context& sema_context,
                                        facade::cmake_facade& cmake_facade)
    : m_function_caller{ caller }
    , m_sema_context{ sema_context }
    , m_cmake_facade{ cmake_facade }
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
    inst::instances_holder instances{ m_sema_context };
    expression_evaluation_context ctx{ m_function_caller, instances, *this,
                                       m_cmake_facade };
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
    // Todo: implement initialization of static members, when static members
    // are implemented.
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

  std::unordered_map<unsigned, std::unique_ptr<inst::instance>>
  gather_instances()
  {
    return std::move(m_instances);
  }

private:
  function_caller& m_function_caller;
  const sema::sema_context& m_sema_context;
  facade::cmake_facade& m_cmake_facade;
  std::unordered_map<unsigned, std::unique_ptr<inst::instance>> m_instances;
};
}
