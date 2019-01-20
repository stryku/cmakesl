#pragma once

#include "common/assert.hpp"
#include "ast/function.hpp"
#include "ast/builtin_function_kind.hpp"

namespace cmsl
{
    namespace ast
    {
        // todo rename to builtin
        class builtin_function : public function
        {
        public:
            // todo accept name as parameter
            explicit builtin_function(const type& return_type, builtin_function_kind k, params_declaration_t params_declaration)
                : m_return_type{ return_type }
                 ,   m_kind{ k }
                , m_params_declaration{ std::move(params_declaration) }
            {}

            cmsl::string_view get_name() const override
            {
                // todo introduce factory
                switch(m_kind)
                {
                    case builtin_function_kind::size: return "size";
                    case builtin_function_kind::empty: return "empty";
                    case builtin_function_kind::push_back: return "push_back";
                    case builtin_function_kind::version_ctor: return "version";
                    case builtin_function_kind::project_ctor: return "project";
                    case builtin_function_kind::cmake_minimum_required: return "cmake_minimum_required";
                    case builtin_function_kind::project_add_executable: return "add_executable";

                    default:
                        CMSL_UNREACHABLE("Passed unknown builtin function kind");
                }
            }

            builtin_function_kind get_fundamental_fun_kind() const
            {
                return m_kind;
            }

            const params_declaration_t& get_params_declarations() const override
            {
                return m_params_declaration;
            }

            const type& get_type() const override
            {
                return m_return_type;
            }

        private:
            const type& m_return_type;
            builtin_function_kind m_kind;
            params_declaration_t m_params_declaration;
        };
    }
}
