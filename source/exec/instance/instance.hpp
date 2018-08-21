#pragma once

#include "common/string.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class function_node;
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
                    fundamental,
                    user
                };

                virtual ~instance() {}

                virtual std::unique_ptr<instance> copy() const = 0;
                virtual int get_value() const = 0;
                virtual void assign(int val) = 0;
                virtual instance* get_member(cmsl::string_view name) = 0;
                virtual bool has_function(cmsl::string_view name) const = 0;
                virtual const ast::function_node* get_function(cmsl::string_view name) const = 0;
            };
        }
    }
}
