#pragma once

#include <memory>

namespace cmsl::sema {

class expression_node;
class block_node;

class block_node_manipulator
{
public:
  explicit block_node_manipulator(block_node& block);

  void append_expression(std::unique_ptr<expression_node> node);

private:
  block_node& m_block_node;
};
}