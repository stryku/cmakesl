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
                size,
                empty,
                push_back
            };

            explicit fundamental_function(fundamental_function_kind k, params_declaration_t params_declaration)
                : m_kind{ k }
                , m_params_declaration{ std::move(params_declaration) }
            {}

            cmsl::string_view get_name() const override
            {
                switch(m_kind)
                {
                    case fundamental_function_kind::size: return "size";
                    case fundamental_function_kind::empty: return "empty";
                    case fundamental_function_kind::push_back: return "push_back";
                }
            }

            fundamental_function_kind get_fundamental_fun_kind() const
            {
                return m_kind;
            }

            const params_declaration_t& get_params_declarations() const override
            {
                return m_params_declaration;
            }

        private:
            fundamental_function_kind m_kind;
            params_declaration_t m_params_declaration;
        };
    }
}
