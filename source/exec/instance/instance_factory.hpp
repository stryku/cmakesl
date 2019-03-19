#pragma once

#include "common/string.hpp"
#include "exec/instance/instance_members_alias.hpp"
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

            class instance_factory
            {
            public:
                std::unique_ptr<instance> create(instance_value_t value, const ast::ast_context &ast_ctx) const;
                //std::unique_ptr<instance> create(instance_members_t value) const;
                std::unique_ptr<instance> create(cmsl::string_view name, execution_context &exec_ctx) const;
                std::unique_ptr<instance> create(cmsl::string_view name, instance_value_t value, execution_context &exec_ctx) const;
                std::unique_ptr<instance> create(const ast::type& type, instance_members_t members) const;
                std::unique_ptr<instance> create(const ast::type& type) const;
            };

            class contexted_instance_factory
            {
            public:
                explicit contexted_instance_factory(const ast::ast_context &ast_ctx, execution_context &exec_ctx);

                std::unique_ptr<instance> create(instance_value_t value) const;
                //std::unique_ptr<instance> create(instance_members_t value) const;
                std::unique_ptr<instance> create(cmsl::string_view name) const;
                std::unique_ptr<instance> create(cmsl::string_view name, instance_value_t value) const;
                std::unique_ptr<instance> create(const ast::type& type, instance_members_t members) const;
                std::unique_ptr<instance> create(const ast::type& type) const;

            private:
                const ast::ast_context &m_ast_ctx;
                execution_context &m_exec_ctx;
            };

            //////////////////////////////////////

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
