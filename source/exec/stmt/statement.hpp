#pragma once

namespace cmsl
{
    namespace exec
    {
        class source_executor;

        namespace stmt
        {
            class statement
            {
            public:
                virtual void execute(source_executor& e) = 0;
            };
        }
    }
}
