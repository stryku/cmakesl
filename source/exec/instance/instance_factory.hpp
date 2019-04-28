#pragma once

#include "common/string.hpp"
#include "exec/instance/instance_value.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class ast_context;
        class type;
    }

    namespace sema
    {
        class sema_context_interface;
        class sema_type;
    }

    namespace exec
    {
        class execution_context;

        namespace inst
        {
            class instance;

            class instance_factory2
            {
            public:
                std::unique_ptr<instance> create(instance_value_t value, const sema::sema_context_interface &ctx) const;
                std::unique_ptr<instance> create_reference(instance& referenced_instance) const;
                std::unique_ptr<instance> create(const sema::sema_type& type) const;
                std::unique_ptr<instance> create(const sema::sema_type& type, instance_value_t value) const;
            };
        }
    }
}
