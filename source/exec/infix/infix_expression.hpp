#pragma once

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance;
        }

        namespace infix
        {
            class infix_expression
            {
            public:
                virtual ~infix_expression() = default;

                virtual inst::instance* evaluate() = 0;
            };
        }
    }
}
