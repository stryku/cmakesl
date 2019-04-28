#pragma once

#include "exec/instance/instance_value.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class type;
    }

    namespace sema
    {
        class sema_type;
    }

    namespace exec::inst
    {
            class instance;

            // Interface of a class that handles lifetime of 'rvalue' instances created during full expression execution
            class instances_holder_interface
            {
            public:
                virtual void store(std::unique_ptr<instance> i) = 0; // Todo: consider renaming to e.g. take_ownership
                virtual std::unique_ptr<instance> gather_ownership(inst::instance *instance_ptr) = 0;

                virtual inst::instance *create2(instance_value_t value) = 0;
                virtual inst::instance *create2_reference(inst::instance& referenced_instance) = 0;
                virtual inst::instance *create2(const sema::sema_type& type) = 0;
                virtual inst::instance *create2(const sema::sema_type& type, instance_value_t value) = 0;
                virtual inst::instance *create2_void() = 0;

            };
    }
}
