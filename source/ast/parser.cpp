#include "ast/parser.hpp"
#include "ast/type.hpp"

#include "common/algorithm.hpp"

#include "errors/error.hpp"
#include "errors/errors_observer.hpp"

namespace cmsl
{
    namespace ast
    {
        parser::parser(errors::errors_observer& err_observer, const lexer::token::token_container_t& tokens)
            : m_err_observer{ err_observer }
            , m_token{ tokens.cbegin() }
            , m_end{ tokens.cend() }
        {}

        void parser::raise_error()
        {
            m_err_observer.nofify_error(errors::error{});
        }

        bool parser::eat(boost::optional<lexer::token::token_type> type)
        {
            if (type && m_token->get_type() != *type)
            {
                raise_error();
                return false;
            }

            ++m_token;
            return true;
        }

        bool parser::eat_type()
        {
            const auto token_type = m_token->get_type();

            if (is_builtin_type(token_type) ||
                token_type == token_type_t::identifier)
            {
                eat();
                return true;
            }
            else
            {
                raise_error();
                return false;
            }
        }

        bool parser::is_builtin_type(token_type_t token_type) const
        {
            const auto builtin_types = {
                token_type_t::t_int,
                token_type_t::t_real
            };

            return cmsl::contains(builtin_types, token_type);
        }

        boost::optional<type> parser::get_type()
        {
            const auto t = *m_token;

            if (!eat_type())
            {
                return boost::none;
            }

            return type{ t };
        }

        std::vector<parameter_declaration> parser::get_parameters_declaration()
        {
            std::vector<parameter_declaration> params;

            if (!eat(token_type_t::open_paren))
            {
                return{};
            }

            while (m_token->get_type() != token_type_t::close_paren)
            {
                const auto param_decl = get_parameter_declaration();
                if (!param_decl)
                {
                    return{};
                }

                params.push_back(std::move(*param_decl));

                if (!prepare_for_next_parameter_declaration())
                {
                    return{};
                }
            }

            if(!eat(token_type_t::close_paren))
            {
                return{};
            }

            return params;
        }

        bool parser::prepare_for_next_parameter_declaration()
        {
            if (m_token->get_type() == token_type_t::semicolon)
            {
                if (!eat(token_type_t::semicolon))
                {
                    return false;
                }

                if (m_token->get_type() == token_type_t::close_paren)
                {
                    // Missed last parameter declaration
                    raise_error();
                    return false;
                }
            }

            return true;
        }

        boost::optional<parameter_declaration> parser::get_parameter_declaration()
        {
            auto t = get_type();

            if (!t)
            {
                return{};
            }

            auto name = *m_token;
            if (!eat(token_type_t::identifier))
            {
                return{};
            }
            
            return parameter_declaration{ *t, name };
        }
    }
}
