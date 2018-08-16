#pragma once

#include "ast/type.hpp"

#include <boost/variant.hpp>

namespace cmsl
{
    namespace exec
    {
        class instance
        {
        private:
            using members_t = string_view_map<std::unique_ptr<instance>>;
            using value_t = int;
            using data_t = boost::variant<members_t, value_t>;

        public:
            enum class kind
            {
                fundamental,
                user
            };

            explicit instance(kind k, const ast::type& type);
            explicit instance(const ast::type& type, int value);
            explicit instance(const ast::type& type, members_t m_members);
            virtual ~instance() {}

            virtual int get_value() const;
            virtual void assign(int val);

            instance* get_member(cmsl::string_view name);

            bool is_fundamental() const;

        private:
            data_t get_init_data() const;
            void expect_fundamental() const;
            void expect_user() const;

        private:
            kind m_kind;
            const ast::type& m_type;
            data_t m_data;
        };
    }
}
