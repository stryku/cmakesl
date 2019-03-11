#pragma once

#include <sema/sema_function.hpp>
#include "ast/type.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instance_members_alias.hpp"

namespace cmsl
{
    namespace sema
    {
        class sema_type;
    }

    namespace exec
    {
        namespace inst
        {
            class complex_unnamed_instance : public instance
            {
            public:
                explicit complex_unnamed_instance(const ast::type &type);
                explicit complex_unnamed_instance(kind k, const ast::type &type);
                explicit complex_unnamed_instance(const ast::type &type, instance_members_t members);
                virtual ~complex_unnamed_instance() = default;

                explicit complex_unnamed_instance(const sema::sema_type &type);
                explicit complex_unnamed_instance(const sema::sema_type &type, instance_members_t members);

                instance_value_t get_value() const override;
                instance_value_t& get_value_ref() override;
                const instance_value_t& get_value_cref() const override;
                void assign(instance_value_t val) override;
                std::unique_ptr<instance> copy() const override;
                instance *get_member(cmsl::string_view name) override;
                const instance *get_cmember(cmsl::string_view name) const override;
                bool has_function(cmsl::string_view name) const override;
                bool is_ctor(cmsl::string_view name) const override; // todo should not be in instance
                bool is_fundamental() const;
                const ast::function* get_function(cmsl::string_view name) const override;
                const ast::type& get_type() const override;
                sema::single_scope_function_lookup_result_t get_sema_function(lexer::token::token name) const override;
                const sema::sema_type& get_sema_type() const override;

            private:
                instance_members_t get_init_data() const;
                instance_members_t get_init_data(instance_members_t m_members) const;

                instance_members_t copy_members() const;
                instance_members_t create_init_members() const;
                instance_members_t create_init_members_sema() const; // Todo: rename

            private:
                kind m_kind;
                const ast::type *m_type{ nullptr };
                const sema::sema_type* m_sema_type{ nullptr }; // Todo: reference instead of pointer
                instance_members_t m_members;
            };
        }
    }
}
