#include "exec/function_call_evaluator.hpp"

#include "exec/execution.hpp"

namespace cmsl
{
    namespace exec
    {
        inst::instance *
        function_call_evaluator::call(const ast::function &fun, const std::vector<inst::instance *> &params)
        {
            return nullptr;
        }

        inst::instance *
        function_call_evaluator::call_member(inst::instance &class_instance, const ast::function &fun, const std::vector<inst::instance *> &params)
        {
            return nullptr;
        }
    }
}
