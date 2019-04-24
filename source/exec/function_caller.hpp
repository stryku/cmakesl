#pragma once

#include <memory>
#include <vector>

namespace cmsl
{
    namespace ast
    {
        class function;
    }

    namespace sema
    {
        class sema_function;
    }

    namespace exec
    {
        namespace inst
        {
            class instance;
            class instances_holder_interface;
        }

        // Todo: change name
        class function_caller2
        {
        public:
            virtual ~function_caller2() = default;

            virtual std::unique_ptr<inst::instance> call(const sema::sema_function& fun,
                                                         const std::vector<inst::instance*>& params,
                                                         inst::instances_holder_interface& instances) = 0;
            virtual std::unique_ptr<inst::instance> call_member(inst::instance& class_instance,
                                                                const sema::sema_function& fun,
                                                                const std::vector<inst::instance*>& params,
                                                                inst::instances_holder_interface& instances) = 0;
        };
    }
}

