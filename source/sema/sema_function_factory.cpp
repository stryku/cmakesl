#include "sema/sema_function_factory.hpp"
#include "sema/sema_function_impl.hpp"

namespace cmsl
{
    namespace sema
    {
        sema_function_impl &
        sema_function_factory::create(const sema_context_interface &ctx, const sema_type &return_type, sema_function::function_signature s)
        {
            auto fun = std::make_unique<sema_function_impl>(ctx, return_type, std::move(s));
            auto ptr = fun.get();
            m_functions.emplace_back(std::move(fun));
            return *ptr;
        }
    }
}
