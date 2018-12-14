#pragma once

#include "ast/type.hpp"
#include "exec/instance/instance.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class simple_unnamed_instance : public instance
            {
            public:
                explicit simple_unnamed_instance(const ast::type &type);
                explicit simple_unnamed_instance(kind k, const ast::type &type);
                explicit simple_unnamed_instance(const ast::type &type, instance_value_t value);
                virtual ~simple_unnamed_instance() = default;

                instance_value_t get_value() const override;
                instance_value_t& get_value_ref() override;
                const instance_value_t& get_value_cref() const override;
                void assign(instance_value_t val) override;
                std::unique_ptr<instance> copy() const override;
                instance *get_member(cmsl::string_view name) override;
                const instance *get_cmember(cmsl::string_view name) const override;
                bool has_function(cmsl::string_view name) const override;
                bool is_ctor(cmsl::string_view name) const override;
                bool is_fundamental() const;
                const ast::function* get_function(cmsl::string_view name) const override;
                const ast::type& get_type() const override;

            private:
                instance_value_t get_init_data() const;
                instance_value_t get_init_data(instance_value_t val) const;
                instance_value_t get_init_list_data() const;

            private:
                kind m_kind;
                const ast::type &m_type;
                instance_value_t m_data;
            };
        }
    }
}
