#pragma once

// Todo: is it needed?
namespace cmsl
{
    namespace sema
    {
        class sema_context;
    }

    namespace exec
    {
        class execution_context;

        class context_provider
        {
        public:
            virtual ~context_provider() {}
            //virtual const sema::sema_context& get_sema_ctx() const = 0;
            virtual execution_context& get_exec_ctx() = 0;
        };
    }
}