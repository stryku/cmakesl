#pragma once

namespace cmsl
{
    namespace ast
    {
        enum class expression_type
        {
            infix,
            declaration,
            ret,
            function,
            block
        };
    }
}
