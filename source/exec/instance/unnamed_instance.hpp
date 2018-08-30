#pragma once

#include "ast/type.hpp"
#include "exec/instance/instance.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class unnamed_instance : public instance
            {
            private:
                using members_t = string_view_map<std::unique_ptr<instance>>;
                using data_t = boost::variant<members_t, instance_value_t>;

            public:
                explicit unnamed_instance(const ast::type &type);
                explicit unnamed_instance(kind k, const ast::type &type);
                explicit unnamed_instance(const ast::type &type, instance_value_t value);
                explicit unnamed_instance(const ast::type &type, members_t members);
                virtual ~unnamed_instance() {}

                instance_value_t get_value() const override;
                void assign(instance_value_t val) override;
                std::unique_ptr<instance> copy() const override;
                instance *get_member(cmsl::string_view name) override;
                bool has_function(cmsl::string_view name) const override;
                bool is_fundamental() const;
                const ast::function_node* get_function(cmsl::string_view name) const override;
                const ast::type& get_type() const override;

            private:
                data_t get_init_data() const;
                data_t get_init_data(instance_value_t val) const;
                data_t get_init_data(members_t m_members) const;
                void expect_fundamental() const;
                void expect_user() const;

                members_t copy_members() const;
                members_t create_init_members() const;

            private:
                kind m_kind;
                const ast::type &m_type;
                data_t m_data;
            };
        }
    }
}
