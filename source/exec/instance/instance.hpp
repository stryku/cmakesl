#pragma once

#include "common/string.hpp"
#include "exec/instance/instance_value.hpp"
#include "sema/function_lookup_result.hpp"

#include <memory>

namespace cmsl
{
    namespace lexer
    {
        namespace token
        {
            class token;
        }
    }

    namespace sema
    {
        class sema_function;
        class sema_type;
    }

    namespace exec
    {
        namespace inst
        {
            class instance
            {
            public:
                enum class kind
                {
                    builtin,
                    user
                };

                virtual ~instance() = default;

                virtual std::unique_ptr<instance> copy() const = 0;
                virtual instance_value_t get_value() const = 0;
                virtual instance_value_t& get_value_ref() = 0;
                virtual const instance_value_t& get_value_cref() const = 0;
                virtual void assign(instance_value_t val) = 0;
                virtual instance* get_member(cmsl::string_view name) = 0;
                virtual const instance* get_cmember(cmsl::string_view name) const = 0;
                virtual sema::single_scope_function_lookup_result_t get_sema_function(lexer::token::token name) const = 0;
                virtual const sema::sema_type& get_sema_type() const = 0;
            };
        }
    }
}
