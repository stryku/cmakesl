#pragma once

namespace cmsl
{
    namespace ast
    {
        class expression
        {
        public:
            virtual void evaluate() = 0;
        };
    }
}
