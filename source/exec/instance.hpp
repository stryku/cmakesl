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

            virtual int get_value() const;
            virtual void assign(int val);

        private:
            const ast::type& m_type;
            int m_value;
        };
    }
}
