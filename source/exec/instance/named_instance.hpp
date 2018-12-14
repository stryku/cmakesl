#pragma once

#include "ast/type.hpp"
#include "exec/instance/instance.hpp"

namespace cmsl
{
    namespace exec
    {
        class execution_context;

        namespace inst
        {
            class named_instance : public instance
            {
            public:
                named_instance(cmsl::string_view name, execution_context &ctx);
                named_instance(cmsl::string_view name, instance& referenced_instance);

                instance_value_t get_value() const override;
                instance_value_t& get_value_ref() override;
                const instance_value_t& get_value_cref() const override;
                void assign(instance_value_t val) override;
                std::unique_ptr<instance> copy() const override;
                instance* get_member(cmsl::string_view name) override;
                const instance* get_cmember(cmsl::string_view name) const override;
                bool has_function(cmsl::string_view name) const override;
                bool is_ctor(cmsl::string_view name) const override;
                const ast::function* get_function(cmsl::string_view name) const override;
                const ast::type& get_type() const override;

            private:
                cmsl::string_view m_name;
                instance& m_instance;
            };
        }
    }
}
