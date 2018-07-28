#include "ast/parser.hpp"
#include "ast/type.hpp"

namespace cmsl
{
    namespace ast
    {
        parser::parser(token_it token)
            : m_token{ token }
        {}

        void parser::raise_error()
        {
            throw std::runtime_error{ "parsing error" };
        }

        void parser::eat(boost::optional<lexer::token::token_type> type)
        {
            if (type && m_token->get_type() != *type)
            {
                raise_error();
            }

            ++m_token;
        }

        void parser::eat_type()
        {
            const auto token_type = m_token->get_type();

            if (is_builtin_type(token_type) ||
                token_type == token_type_t::identifier)
            {
                eat();
            }
            else
            {
                raise_error();
            }
        }

        bool parser::is_builtin_type(token_type_t token_type) const
        {
            const auto builtin_types = {
                token_type_t::t_int,
                token_type_t::t_real
            };

            return std::any_of(std::cbegin(builtin_types), std::cend(builtin_types),
                               [token_type](const auto builtin_type)
                               {
                                   return token_type == builtin_type;
                               });
        }

        type parser::get_type()
        {
            const auto t = *m_token;
            eat_type();
            return type{ t };
        }

        std::vector<parameter_declaration> parser::get_parameters_declaration()
        {
            std::vector<parameter_declaration> params;

            eat(token_type_t::open_paren);

            while (m_token->get_type() != token_type_t::close_paren)
            {
                auto t = get_type();
                auto name = *m_token;
                eat(token_type_t::identifier);

                if (m_token->get_type() == token_type_t::semicolon)
                {
                    eat(token_type_t::semicolon);
                } // todo handle semicolon without param

                params.push_back({ t, name });
            }

            eat(token_type_t::close_paren);

            return params;
        }
    }
}
