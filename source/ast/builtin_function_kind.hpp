#pragma once

namespace cmsl
{
    namespace ast
    {
        enum class builtin_function_kind
        {
            size,
            empty,
            push_back,

            version_ctor,

            cmake_minimum_required
        };
    }
}
