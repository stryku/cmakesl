#pragma once

namespace cmsl
{
    namespace ast
    {
        enum class ast_node_type
        {
            infix,
            declaration,
            ret,
            function,
            block,
            class_type,
            if_else,
            conditional,
            while_
        };
    }
}
