#pragma once

#include "exec/instance/instances_holder_interface.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace ast
    {
        class type;
    }

    namespace sema
    {
        class sema_context_interface;
    }

    namespace exec
    {
        class context_provider;

        namespace inst
        {
            class instance;
            class contexted_instance_factory;

            // class used to handle lifetime of 'rvalue' instances created during full expression execution
            class instances_holder : public instances_holder_interface
            {
            public:
                explicit instances_holder(const sema::sema_context_interface& sema_ctx);
                explicit instances_holder(context_provider& e);

                inst::instance* create(instance_value_t value) override;
                inst::instance* create(const ast::type& type, instance_members_t members) override;
                inst::instance* create(const ast::type& t) override;
                inst::instance* create_void() override;

                void store(std::unique_ptr<instance> i) override;

                std::unique_ptr<instance> gather_ownership(inst::instance* instance_ptr) override;

                inst::instance* create2(instance_value_t value) override;
                inst::instance *create2_reference(inst::instance& referenced_instance) override;

            private:
                contexted_instance_factory get_factory();

            private:
                const sema::sema_context_interface* m_sema_ctx; // Todo: change to a reference.
                context_provider* m_ctx_provider;
                std::vector<std::unique_ptr<inst::instance>> m_instances;
            };
        }
    }
}
