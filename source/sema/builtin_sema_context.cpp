#include "sema/builtin_sema_context.hpp"
#include "sema/factories.hpp"
#include "sema/type_builder.hpp"

namespace cmsl
{
    namespace sema
    {
        builtin_sema_context::builtin_sema_context(sema_type_factory &type_factory,
                                                   sema_function_factory &function_factory,
                                                   sema_context_factory &context_factory)
            : m_type_factory{ type_factory }
            , m_function_factory{ function_factory }
            , m_context_factory{ context_factory }
        {
            add_types();
            add_functions();
        }

        void builtin_sema_context::add_types()
        {
            add_bool_type();

        }

        void builtin_sema_context::add_functions()
        {

        }

        void builtin_sema_context::add_bool_type()
        {
        }
    }
}