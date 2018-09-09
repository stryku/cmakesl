#pragma once

#include "exec/instance/instance_members_alias.hpp"
#include "exec/instance/instance_value.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace ast
    {
        class type;
    }

    namespace exec
    {
        class context_provider;

        namespace inst
        {
            class instance;
            class instance_factory;

            // class used to handle lifetime of 'rvalue' instances created during full expression execution
            class instances_holder
            {
            public:
                explicit instances_holder(context_provider& e);

                inst::instance* create(instance_value_t value);
                inst::instance* create(const ast::type& type, instance_members_t members);
                inst::instance* create(const ast::type& t);

                void store(std::unique_ptr<instance> i);

                std::unique_ptr<instance> gather_ownership(inst::instance* instance_ptr);

            private:
                inst::instance_factory get_factory();

            private:
                context_provider& m_ctx_provider;
                std::vector<std::unique_ptr<inst::instance>> m_instances;
            };
        }
    }
}
