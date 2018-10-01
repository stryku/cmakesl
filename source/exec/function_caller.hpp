#pragma once

#include <vector>

namespace cmsl
{
    namespace ast
    {
        class function;
    }

    namespace exec
    {
        namespace inst
        {
            class instance;
        }

        class function_caller
        {
        public:
            virtual ~function_caller() = default;

            virtual inst::instance* call(const ast::function& fun, const std::vector<inst::instance*>& params) = 0;
            virtual inst::instance* call_member(inst::instance& class_instance,
                                                const ast::function& fun,
                                                const std::vector<inst::instance*>& params) = 0;
        };
    }
}

