#pragma once

#include "exec/instance/instance.hpp"

namespace cmsl::exec
{
        class execution_context;

        namespace inst
        {
            class instance_reference : public instance
            {
            public:
                instance_reference(cmsl::string_view name, execution_context &ctx);
                instance_reference(instance& referenced_instance);

                instance_value_t get_value() const override;
                instance_value_t& get_value_ref() override;
                const instance_value_t& get_value_cref() const override;
                void assign(instance_value_t val) override;
                std::unique_ptr<instance> copy() const override;
                instance* get_member(cmsl::string_view name) override;
                const instance* get_cmember(cmsl::string_view name) const override;
                sema::single_scope_function_lookup_result_t get_sema_function(lexer::token::token name) const override;
                const sema::sema_type& get_sema_type() const override;

            private:
                instance& m_instance;
            };
        }
}
