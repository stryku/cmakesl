#pragma once

#include "common/source_location.hpp"
#include "completion_contextes.hpp"
#include "sema/sema_node_visitor.hpp"

#include <variant>

namespace cmsl::tools {
class completion_context_finder : public sema::empty_sema_node_visitor
{
public:
  explicit completion_context_finder(unsigned absolute_position);

  void visit(const sema::translation_unit_node& node) override;
  void visit(const sema::function_node& node) override;
  void visit(const sema::block_node& node) override;
  void visit(const sema::class_node& node) override;

  completion_context_t result() const;

private:
  bool is_inside(const sema::sema_node& node) const;
  bool is_before(const sema::sema_node& node) const;
  bool is_pos_before_node_begin(const sema::sema_node& node) const;

private:
  const source_location m_pos;
  completion_context_t m_result;
};
}
