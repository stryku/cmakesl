#pragma once

#include <sema/sema_function.hpp>
#include "exec/instance/instance.hpp"

namespace cmsl
{
    namespace sema
    {
        class sema_type;
    }

    namespace exec::inst
    {
            class complex_unnamed_instance : public instance
            {
            private:
                using instance_members_t = string_view_map<std::unique_ptr<instance>>;

            public:
                explicit complex_unnamed_instance(const sema::sema_type &type);
                explicit complex_unnamed_instance(const sema::sema_type &type, instance_members_t members);

                instance_value_variant value() const override;
                instance_value_variant& value_ref() override;
                const instance_value_variant& value_cref() const override;
                void assign(instance_value_variant val) override;
                std::unique_ptr<instance> copy() const override;
                instance *find_member(cmsl::string_view name) override;
                const instance *find_cmember(cmsl::string_view name) const override;
                bool is_fundamental() const;
                sema::single_scope_function_lookup_result_t find_function(lexer::token name) const override;
                const sema::sema_type& type() const override;

            private:
                instance_members_t get_init_data() const;
                instance_members_t get_init_data(instance_members_t m_members) const;

                instance_members_t copy_members() const;
                instance_members_t create_init_members() const;

            private:
                kind m_kind;
                const sema::sema_type& m_sema_type;
                instance_members_t m_members;
            };
    }
}
