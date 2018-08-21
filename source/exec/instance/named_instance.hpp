#pragma once

#include "ast/type.hpp"
#include "exec/instance/instance.hpp"

namespace cmsl
{
    namespace exec
    {
        class execution_context;

        namespace inst
        {
            class named_instance : public instance
            {
            public:
                named_instance(cmsl::string_view name,
                               execution_context &ctx);

                int get_value() const override;
                void assign(int val) override;
                std::unique_ptr<instance> copy() const override;
                instance* get_member(cmsl::string_view name) override;
                bool has_function(cmsl::string_view name) const override;

            private:
                cmsl::string_view m_name;
                execution_context &m_ctx;
                instance* m_instance;
            };
        }
    }
}
