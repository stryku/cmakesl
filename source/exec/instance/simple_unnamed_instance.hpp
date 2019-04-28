#pragma once

#include "exec/instance/instance.hpp"

namespace cmsl::exec::inst
{
            // Todo: consider rename to builtin_unnamed_instance
            class simple_unnamed_instance : public instance
            {
            public:
                explicit simple_unnamed_instance(const sema::sema_type &type);
                explicit simple_unnamed_instance(const sema::sema_type &type, instance_value_variant value);


                virtual ~simple_unnamed_instance() = default;

                instance_value_variant get_value() const override;
                instance_value_variant& get_value_ref() override;
                const instance_value_variant& get_value_cref() const override;
                void assign(instance_value_variant val) override;
                std::unique_ptr<instance> copy() const override;
                instance *get_member(cmsl::string_view name) override;
                const instance *get_cmember(cmsl::string_view name) const override;
                bool is_fundamental() const;

                sema::single_scope_function_lookup_result_t get_sema_function(lexer::token::token name) const override; // Todo: rename
                const sema::sema_type& get_sema_type() const override; // Todo: rename

            private:
                instance_value_variant get_init_data() const;
                instance_value_variant get_init_data(instance_value_variant val) const;

            private:
                kind m_kind;
                const sema::sema_type* m_sema_type{nullptr}; // Todo: change to a reference
                instance_value_variant m_data;
            };
}
