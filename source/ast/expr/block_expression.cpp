#include "ast/expr/block_expression.hpp"

namespace cmsl
{
    namespace ast
    {
        block_expression::block_expression(expressions_t exprs)
            : m_expressions{ std::move(exprs) }
        {}

        block_expression::~block_expression()
        {}


        block_expression::result_t block_expression::evaluate()
        {
            return{};
        }
    }
}
