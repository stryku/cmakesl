#include "sema/dumper.hpp"

#include "sema/sema_nodes.hpp"
#include "sema/sema_type.hpp"

namespace cmsl::sema {

std::string dumper::m_ident;

dumper::ident_guard::ident_guard(std::string& ident)
  : m_ident{ ident }
{
  m_ident.push_back(' ');
}

dumper::ident_guard::~ident_guard()
{
  m_ident.pop_back();
}

dumper::dumper(std::ostream& os)
  : m_out{ os }
{
  m_ident.clear();
  out() << "-sema dumping begin";
}

dumper::~dumper()
{
  m_ident.clear();
  out() << "-sema dumping end";
}

dumper::ident_guard dumper::ident()
{
  return ident_guard{ m_ident };
}

dumper::line_out dumper::out()
{
  return line_out{ m_out };
}

void dumper::visit(const variable_declaration_node& node)
{
  out() << "-variable declaration";
  auto ig = ident();

  dump_type(node.type());

  out() << "-name: " << node.name().str();
  out() << "-index: " << node.index();

  if (auto init = node.initialization()) {
    out() << "-initialization";
    auto ig2 = ident();
    init->visit(*this);
  }
}

void dumper::visit(const bool_value_node& node)
{
  out() << "-bool value: value: " << (node.value() ? "true" : "false");
}

void dumper::visit(const int_value_node& node)
{
  out() << "-int value: value: " << node.value();
}

void dumper::visit(const double_value_node& node)
{
  out() << "-double value: value: " << node.value();
}

void dumper::visit(const string_value_node& node)
{
  out() << "-double value: value: \"" << node.value() << "\"";
}

void dumper::visit(const id_node& node)
{
  out() << "-identifier";
  auto ig = ident();

  dump_qualified_name(node.names());
  out() << "-index: " << node.index();
}

void dumper::visit(const return_node& node)
{
  out() << "-return";
  auto ig = ident();
  out() << "-expression";
  auto expression_ig = ident();
  node.expression().visit(*this);
}

void dumper::visit(const binary_operator_node& node)
{
  out() << "-binary operator: operator: " << node.op().str();
  auto ig = ident();

  dump_type(node.type());

  out() << "-lhs";
  {
    auto ig_lhs = ident();
    node.lhs().visit(*this);
  }

  out() << "-rhs";
  {
    auto ig_rhs = ident();
    node.rhs().visit(*this);
  }
}

void dumper::visit(const function_call_node& node)
{
  out() << "-function call";
  auto ig = ident();

  visit_call_node(node);
}

void dumper::visit(const member_function_call_node& node)
{
  out() << "-member function call";
  auto ig = ident();

  visit_call_node(node);

  {
    out() << "-lhs";
    auto lhs_ig = ident();
    node.lhs().visit(*this);
  }
}

void dumper::visit(const implicit_member_function_call_node& node)
{
  out() << "-implicit member function call";
  auto ig = ident();

  visit_call_node(node);
}

void dumper::visit(const constructor_call_node& node)
{
  out() << "-constructor call";
  auto ig = ident();

  visit_call_node(node);
}

void dumper::visit(const block_node& node)
{
  out() << "-block";
  auto ig = ident();

  out() << "-nodes";
  for (const auto& child : node.nodes()) {
    auto child_ig = ident();
    child->visit(*this);
  }
}

void dumper::visit(const function_node& node)
{
  out() << "-function";
  auto ig = ident();

  out() << "-return type: " << &node.return_type() << " "
        << node.return_type().name().to_string();
  out() << "-name: " << node.signature().name.str();

  {
    out() << "-parameters";
    auto params_ig = ident();

    const auto& params = node.signature().params;
    for (auto i = 0u; i < params.size(); ++i) {
      out() << "-param " << i << "; type: " << &params[i].ty << " "
            << params[i].ty.name().to_string()
            << "; name: " << params[i].name.str();
    }
  }

  {
    out() << "-body";
    auto body_ig = ident();
    node.body().visit(*this);
  }
}

void dumper::visit(const class_node& node)
{
  out() << "-class";
  auto ig = ident();

  out() << "-name: " << node.name().str();

  {
    out() << "-members";
    auto members_ig = ident();
    for (const auto& member : node.members()) {
      member->visit(*this);
    }
  }

  {
    out() << "-functions";
    auto functions_ig = ident();
    for (const auto& fun : node.functions()) {
      fun->visit(*this);
    }
  }
}

void dumper::visit(const conditional_node& node)
{
  out() << "-conditional";
  auto ig = ident();

  {
    out() << "-condition";
    auto condition_ig = ident();
    node.get_condition().visit(*this);
  }

  {
    out() << "-body";
    auto body_ig = ident();
    node.get_body().visit(*this);
  }
}

void dumper::visit(const while_node& node)
{
  out() << "-while";
  auto ig = ident();

  {
    out() << "-condition";
    auto condition_ig = ident();
    node.condition().visit(*this);
  }

  {
    out() << "-body";
    auto body_ig = ident();
    node.body().visit(*this);
  }
}

void dumper::visit(const if_else_node& node)
{
  out() << "-if else";
  auto ig = ident();

  for (const auto& if_ : node.ifs()) {
    out() << "-if";
    auto if_ig = ident();
    if_->visit(*this);
  }

  if (auto else_ = node.else_body()) {
    out() << "-else";
    auto else_ig = ident();
    else_->visit(*this);
  }
}

void dumper::visit(const class_member_access_node& node)
{
  out() << "-class member access";
  auto ig = ident();

  dump_type(node.type());

  {
    out() << "-lhs";
    auto lhs_ig = ident();
    node.lhs().visit(*this);
  }

  out() << "-member name: " << node.member_name().str();
}

void dumper::visit(const translation_unit_node& node)
{
  out() << "-translation unit";
  auto ig = ident();

  for (const auto& child : node.nodes()) {
    child->visit(*this);
  }
}

void dumper::visit(const cast_to_reference_node& node)
{
  out() << "-cast to reference";
  auto ig = ident();

  dump_type(node.type());
  node.expression().visit(*this);
}

void dumper::visit(const cast_to_value_node& node)
{
  out() << "-cast to value";
  auto ig = ident();

  dump_type(node.type());
  node.expression().visit(*this);
}

void dumper::visit(const initializer_list_node& node)
{
  out() << "-initializer list";
  auto ig = ident();

  dump_type(node.type());

  const auto& values = node.values();
  for (auto i = 0u; i < values.size(); ++i) {
    out() << "-value " << i;
    auto value_ig = ident();
    values[i]->visit(*this);
  }
}

void dumper::visit(const add_subdirectory_node& node)
{
  out() << "-add subdirectory";
  auto ig = ident();

  {
    out() << "-directory";
    auto directory_ig = ident();
    node.dir_name().visit(*this);
  }

  visit_call_node(node);
}

void dumper::visit_call_node(const call_node& node)
{
  dump_type(node.type());
  out() << "-function: " << &node.function();
  out() << "-name: " << node.name().str();
  out() << "-parameters";

  {
    auto params_ig = ident();
    const auto& params = node.param_expressions();
    for (auto i = 0u; i < params.size(); ++i) {
      out() << "-param " << i;
      auto param_ig = ident();
      params[i]->visit(*this);
    }
  }
}

void dumper::visit(const implicit_return_node& node)
{
  out() << "-implicit return";
  auto ig = ident();
  dump_type(node.type());
}

void dumper::visit(const for_node& node)
{
  out() << "-for";
  auto ig = ident();

  if (auto init = node.init()) {
    out() << "-init";
    auto init_ig = ident();
    init->visit(*this);
  } else {
    out() << "-init: empty";
  }

  if (auto condition = node.condition()) {
    out() << "-condition";
    auto condition_ig = ident();
    condition->visit(*this);
  } else {
    out() << "-condition: empty";
  }

  if (auto iteration = node.iteration()) {
    out() << "-iteration";
    auto iteration_ig = ident();
    iteration->visit(*this);
  } else {
    out() << "-iteration: empty";
  }

  out() << "-body";
  auto body_ig = ident();
  node.body().visit(*this);
}

void dumper::visit(const break_node&)
{
  out() << "-break";
}

void dumper::dump_type(const sema_type& type)
{
  out() << "-type: " << &type << " " << type.name().to_string();
}

void dumper::visit(const namespace_node& node)
{
  out() << "-namespace";
  auto ig = ident();

  dump_qualified_name(node.names());

  {
    out() << "-nodes";
    auto nodes_ig = ident();
    for (const auto& child : node.nodes()) {
      child->visit(*this);
    }
  }
}

void dumper::dump_qualified_name(
  const std::vector<ast::name_with_coloncolon>& names)
{
  std::string name_access;
  for (const auto& name_with_coloncolon : names) {
    name_access += name_with_coloncolon.name.str();
    if (name_with_coloncolon.coloncolon) {
      name_access += "::";
    }
  }

  out() << "-qualified name: " << name_access;
}

void dumper::visit(const ternary_operator_node& node)
{
  out() << "-ternary operator";
  auto ig = ident();

  {
    out() << "-condition";
    auto guard = ident();
    node.condition().visit(*this);
  }
  {
    out() << "-true value";
    auto guard = ident();
    node.true_().visit(*this);
  }
  {
    out() << "-false value";
    auto guard = ident();
    node.false_().visit(*this);
  }
}

void dumper::visit(const designated_initializers_node& node)
{
  out() << "-designated initializers";
  auto ig = ident();

  const auto& inits = node.initializers();
  for (auto i = 0; i < inits.size(); ++i) {
    const auto& init = inits[i];
    out() << "-initializer " << i;
    auto guard = ident();
    out() << "-name: " << init.name.str();
    out() << "-value";
    auto val_guard = ident();
    init.init->visit(*this);
  }
}

}
