#pragma once

namespace cmsl::ast
{
    // Todo: is this needed?
    enum class ast_node_type
    {
        infix,
        declaration,
        ret,
        function,
        block,
        class_type,
        if_else,
        while_
    };
}
