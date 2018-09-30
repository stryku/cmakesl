#pragma once

#include "exec/function_caller.hpp"

namespace cmsl
{
    namespace exec
    {
        class function_call_evaluator : public function_caller
        {
        public:
            inst::instance* call(const ast::function& fun, const std::vector<inst::instance*>& params) override;
            inst::instance* call_member(inst::instance& class_instance,
                                                const ast::function& fun,
                                                const std::vector<inst::instance*>& params) override;

        };
    }
}
