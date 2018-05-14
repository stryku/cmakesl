#pragma once

#include "ast\expr\expression.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace ast
    {
        class ast_builder;

        class block_expression : public expression
        {
        private:
            using expressions_t = std::vector<std::unique_ptr<expression>>;

        public:
            block_expression(expressions_t exprs);

        private:
            std::vector<std::unique_ptr<expression>> m_expressions;
        };
    }
}
