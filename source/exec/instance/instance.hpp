#pragma once

#include "common/string.hpp"
#include "exec/instance/instance_value.hpp"

#include <boost/variant.hpp>

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class function;
        class type;
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
                virtual void assign(instance_value_t val) = 0;
                virtual instance* get_member(cmsl::string_view name) = 0;
                virtual bool has_function(cmsl::string_view name) const = 0;
                virtual const ast::function* get_function(cmsl::string_view name) const = 0;
                virtual const ast::type& get_type() const = 0;
            };
        }
    }
}
