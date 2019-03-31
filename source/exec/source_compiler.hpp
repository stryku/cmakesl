#pragma once

#include <memory>

namespace cmsl
{
    class source_view;

    namespace errors
    {
        class errors_observer;
    }

    namespace sema
    {
        class sema_type_factory;
        class sema_function_factory;
        class sema_context_factory;
    }

    namespace exec
    {
        class compiled_source;

        class source_compiler
        {
        public:
            explicit source_compiler(errors::errors_observer& errors_observer,
                                     sema::sema_type_factory& type_factory,
                                     sema::sema_function_factory& function_factory,
                                     sema::sema_context_factory& context_factory);

            std::unique_ptr<compiled_source> compile(source_view source);

        private:
            errors::errors_observer& m_errors_observer;
            sema::sema_type_factory& m_type_factory;
            sema::sema_function_factory& m_function_factory;
            sema::sema_context_factory& m_context_factory;
        };
    }
}
