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
    inst::instance& component_instance,
    errors::errors_observer& errors_observer);

  void visit(const decl_sema::bool_value_node& node) override;
  void visit(const decl_sema::cmake_variable_access_node& node) override;
  void visit(const decl_sema::component_declaration_node& node) override;
  void visit(const decl_sema::component_node& node) override;
  void visit(const decl_sema::double_value_node& node) override;
  void visit(const decl_sema::import_node& node) override;
  void visit(const decl_sema::int_value_node& node) override;
  void visit(const decl_sema::list_node& node) override;
  void visit(const decl_sema::property_access_node& node) override;
  void visit(const decl_sema::property_append_node& node) override;
  void visit(const decl_sema::property_node& node) override;
  void visit(const decl_sema::string_value_node& node) override;
  void visit(const decl_sema::translation_unit_node& node) override;

private:
  std::unique_ptr<inst::instance> convert_cmake_variable_to_instance_of_type(
    const std::string& cmake_variable, const sema::sema_type& desired_type);

  declarative_component_property_instances_collecting_visitor clone();

  std::unique_ptr<exec::inst::instance> evaluate_child(
    const decl_sema::expression_node& node);

  inst::instance& access_property_instance(
    inst::instance& component_instance,
    const decl_sema::property_access_node& node);

private:
  facade::cmake_facade& m_facade;
  const sema::builtin_types_accessor& m_builtin_types;
  inst::instances_holder_interface& m_instances;
  sema::generic_type_creation_utils m_generic_types;
  inst::instance& m_component_instance;
  errors::errors_observer& m_errs;

  std::unique_ptr<exec::inst::instance> m_result;
};
}
