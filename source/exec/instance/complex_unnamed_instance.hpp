#pragma once

#include "ast/type.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instance_members_alias.hpp"

namespace cmsl
{
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

                instance_value_t get_value() const override;
                instance_value_t& get_value_ref() override;
                void assign(instance_value_t val) override;
                std::unique_ptr<instance> copy() const override;
                instance *get_member(cmsl::string_view name) override;
                bool has_function(cmsl::string_view name) const override;
                bool is_ctor(cmsl::string_view name) const override; // todo should not be in instance
                bool is_fundamental() const;
                const ast::function* get_function(cmsl::string_view name) const override;
                const ast::type& get_type() const override;

            private:
                instance_members_t get_init_data() const;
                instance_members_t get_init_data(instance_members_t m_members) const;

                instance_members_t copy_members() const;
                instance_members_t create_init_members() const;

            private:
                kind m_kind;
                const ast::type &m_type;
                instance_members_t m_members;
            };
        }
    }
}
