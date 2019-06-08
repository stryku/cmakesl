#include "sema/block_node_manipulator.hpp"
#include "sema/sema_nodes.hpp"

namespace cmsl::sema {

block_node_manipulator::block_node_manipulator(block_node& block)
  : m_block_node{ block }
{
}
void block_node_manipulator::append_expression(
  std::unique_ptr<expression_node> node)
{
  m_block_node.m_nodes.emplace_back(std::move(node));
}
}
