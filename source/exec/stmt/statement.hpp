#pragma once

namespace cmsl
{
    namespace exec
    {
        class execution;

        namespace stmt
        {
            class statement
            {
            public:
                virtual void execute(execution& e) = 0;
            };
        }
    }
}
