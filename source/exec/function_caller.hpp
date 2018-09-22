#pragma once

#include <vector>

namespace cmsl
{
    namespace ast
    {
        class function;
    }

    namespace inst
    {
        class instance;
    }

    namespace exec
    {
        class function_caller
        {
        public:
            virtual ~function_caller() = default;

            virtual inst::instance* call(const ast::function& fun, const std::vector<inst::instance*>& params) = 0;
        };
    }
}

