#pragma once

#include "ast/type.hpp"

namespace cmsl
{
    namespace exec
    {
        class instance
        {
        public:
            explicit instance(const ast::type& type);
            explicit instance(const ast::type& type, int value);

            virtual int get_value() const;
            virtual void assign(int val);

            instance* get_member(cmsl::string_view name);

        private:
            const ast::type& m_type;
            int m_value;
            string_view_map<std::unique_ptr<instance>> m_members;
        };
    }
}
