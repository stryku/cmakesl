#pragma once

#include "exec/instance.hpp"

namespace cmsl
{
    namespace exec
    {
        class execution_context;

        class named_instance : public instance
        {
        public:
            named_instance(const ast::type& t, cmsl::string_view name, execution_context& ctx);

            virtual int get_value() const override;
            virtual void assign(int val) override;

        private:
            cmsl::string_view m_name;
            execution_context& m_ctx;
        };
    }
}
