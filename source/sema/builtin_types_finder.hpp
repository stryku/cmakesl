#pragma once

#include "sema/sema_context.hpp"

namespace cmsl
{
    namespace sema
    {
        class builtin_types_finder
        {
        private:
            using token_type_t = lexer::token::token_type;

        public:
            explicit builtin_types_finder(const sema_context_interface& ctx)
                : m_ctx{ ctx }
            {}

            const sema_type& find_bool() const
            {
                return find<token_type_t::kw_bool>("bool");
            }

            const sema_type& find_int() const
            {
                return find<token_type_t::kw_int>("int");
            }

            const sema_type& find_double() const
            {
                return find<token_type_t::kw_double>("double");
            }

            const sema_type& find_string() const
            {
                return find<token_type_t::kw_string>("string");
            }

            const sema_type& find_version() const
            {
                return find<token_type_t::kw_version>("version");
            }

            const sema_type& find_project() const
            {
                return find<token_type_t::kw_project>("project");
            }

            const sema_type& find_void() const
            {
                return find<token_type_t::kw_void>("void");
            }

        private:
            template<unsigned N>
            lexer::token::token make_token(lexer::token::token_type token_type, const char (&tok)[N]) const
            {
                // N counts also '\0'
                const auto src_range = source_range{
                        source_location{ 1u, 1u, 0u },
                        source_location{ 1u, N, N - 1u }
                };
                return lexer::token::token{ token_type, src_range, tok };
            }

            template <auto token_type, auto N>
            const sema_type& find(const char (&tok)[N]) const
            {
                static const auto token = make_token(token_type, tok);
                static const auto name_reference = ast::type_representation{ token };
                return *m_ctx.find_type(name_reference);
            }



        private:
            const sema_context_interface& m_ctx;
        };
    }
}
