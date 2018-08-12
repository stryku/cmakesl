#include "exec/named_instance.hpp"

#include "exec/execution_context.hpp"

namespace cmsl
{
    namespace exec
    {
        named_instance::named_instance(const ast::type& t,
                                       cmsl::string_view name,
                                       execution_context& ctx)
            : instance{ t }
            , m_name{ name }
            , m_ctx{ ctx }
        {}

        int named_instance::get_value() const
        {
            return *m_ctx.get_variable(m_name);
        }

        void named_instance::assign(int val)
        {
            auto& var = *m_ctx.get_variable(m_name);
            var = val;
        }
    }
}
