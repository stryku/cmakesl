#include "sema/builtin_sema_context.hpp"
#include "sema/factories.hpp"
#include "sema/type_builder.hpp"
#include <algorithm>

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
            const auto token = make_token(token_type_t::kw_bool, "bool");
            type_builder builder{ m_type_factory, m_function_factory, m_context_factory, *this, token };

        }

        template<unsigned N>
        lexer::token::token builtin_sema_context::make_token(lexer::token::token_type token_type, const char (&tok)[N])
        {
            const auto src_range = source_range{
                    source_location{ 1u, 1u, 0u },
                    source_location{ 1u, 1u + N, N }
            };
            return lexer::token::token{ token_type, src_range, tok };
        }
    }
}