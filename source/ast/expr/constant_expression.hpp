#pragma once

#include "ast\expr\expression.hpp"

namespace cmsl
{
    namespace ast
    {
        // Expression that represents constant integrals, bools etc.
        class constant_expression : public expression
        {
        public:
            void evaluate() override;
        };
    }
}
