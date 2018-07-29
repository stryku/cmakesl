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
            if (is_at_end())
            {
                raise_error();
                return false;
            }

            if (type && !current_is(*type))
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

            while (true)
            {
                if (is_at_end())
                {
                    // Unexpected end of tokens
                    raise_error();
                    return{};
                }

                if (current_is(token_type_t::close_paren))
                {
                    // End of parameters
                    break;
                }

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
            if (!expect_not_at_end())
            {
                return false;
            }

            if (!current_is(token_type_t::semicolon))
            {
                return true;
            }

            // At this point we have semicolon, so we expect next parameter

            if (!eat(token_type_t::semicolon))
            {
                return false;
            }

            if (!expect_not_at_end())
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

        bool parser::is_at_end() const
        {
            return m_token == m_end;
        }

        bool parser::expect_token(token_type_t type)
        {
            return expect_not_at_end() && m_token->get_type() == type;
        }

        bool parser::expect_token_other_than(token_type_t type)
        {
            return expect_not_at_end() && m_token->get_type() != type;
        }

        bool parser::expect_not_at_end()
        {
            if (is_at_end())
            {
                raise_error();
                return false;
            }

            return true;
        }

        bool parser::current_is(token_type_t token_type) const
        {
            return m_token->get_type() == token_type;
        }
    }
}
