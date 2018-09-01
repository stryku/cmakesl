#pragma once

#include "ast/function.hpp"

namespace cmsl
{
    namespace ast
    {
        class fundamental_function : public function
        {
        public:
            enum class fundamental_function_kind
            {
                size
            };

            explicit fundamental_function(fundamental_function_kind k)
                : m_kind{ k }
            {}

            cmsl::string_view get_name() const override
            {
                switch(m_kind)
                {
                    case fundamental_function_kind::size: return "size";
                }
            }

            fundamental_function_kind get_fundamental_fun_kind() const
            {
                return m_kind;
            }

        private:
            fundamental_function_kind m_kind;
        };
    }
}
