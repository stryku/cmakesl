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

            project_ctor,
            project_add_executable,

            cmake_minimum_required
        };
    }
}
