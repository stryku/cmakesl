#pragma once

#include "sema/sema_node_visitor.hpp"

#include <ostream>

namespace cmsl::sema {

class call_node;
class sema_type;

class dumper : public sema_node_visitor
{
public:
  explicit dumper(std::ostream& out);

  void visit(const variable_declaration_node&) override;
  void visit(const bool_value_node&) override;
  void visit(const int_value_node&) override;
  void visit(const double_value_node&) override;
  void visit(const string_value_node&) override;
  void visit(const id_node&) override;
  void visit(const return_node&) override;
  void visit(const binary_operator_node&) override;
  void visit(const function_call_node&) override;
  void visit(const member_function_call_node&) override;
  void visit(const implicit_member_function_call_node&) override;
  void visit(const constructor_call_node&) override;
  void visit(const block_node&) override;
  void visit(const function_node&) override;
  void visit(const class_node&) override;
  void visit(const conditional_node&) override;
  void visit(const while_node&) override;
  void visit(const if_else_node&) override;
  void visit(const class_member_access_node&) override;
  void visit(const translation_unit_node&) override;
  void visit(const cast_to_reference_node&) override;
  void visit(const cast_to_value_node&) override;
  void visit(const initializer_list_node&) override;
  void visit(const add_subdirectory_node&) override;
  void visit(const implicit_return_node&) override;
  void visit(const for_node&) override;
  void visit(const break_node&) override;

private:
  class ident_guard
  {
  public:
    explicit ident_guard(std::string& ident);
    ~ident_guard();

    ident_guard(const ident_guard&) = delete;
    ident_guard& operator=(const ident_guard&) = delete;

    ident_guard(ident_guard&&) = default;
    ident_guard& operator=(ident_guard&&) = default;

  private:
    std::string& m_ident;
  };

  ident_guard ident();

  class line_out
  {
  public:
    explicit line_out(std::ostream& os)
      : m_out{ os }
    {
      m_out << m_ident;
    }
    ~line_out() { m_out << '\n'; }

    template <typename T>
    std::ostream& operator<<(T&& value)
    {
      return m_out << std::forward<T>(value);
    }

  private:
    std::ostream& m_out;
  };

  line_out out();

  void visit_call_node(const call_node& node);

  void dump_type(const sema_type& type);

private:
  std::ostream& m_out;
  static std::string m_ident;
};

}
