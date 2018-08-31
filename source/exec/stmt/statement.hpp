#pragma once

namespace cmsl
{
    namespace exec
    {
        class executor;

        namespace stmt
        {
            class statement
            {
            public:
                virtual void execute(executor& e) = 0;
            };
        }
    }
}
