#pragma once

#include "exec/stmt/statement.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace stmt
        {
            class builtin_call_size_statement : public statement
            {
            public:
                 void execute(executor& e) override;
            };
        }
    }
}
