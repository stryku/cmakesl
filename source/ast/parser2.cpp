#include "ast/parser2.hpp"

#include "ast/variable_declaration_node.hpp"

#include "errors/error.hpp"
#include "errors/errors_observer.hpp"

namespace cmsl
{
    namespace ast
    {
        parser2::parser2(errors::errors_observer& err_observer, const token_container_t& tokens)
            : m_err_observer{ err_observer }
            , m_token{ tokens.cbegin() }
            , m_end{ tokens.cend() }
        {}

        void parser2::raise_error()
        {
            m_err_observer.nofify_error(errors::error{});
        }

        std::unique_ptr<ast_node> parser2::translation_unit()
        {
            return std::unique_ptr<ast_node>();
        }

        std::unique_ptr<ast_node> parser2::variable_declaration()
        {
            const auto ty = type();
            if(!ty)
            {
                return nullptr;
            }

            const auto name = eat(token_type_t::identifier);
            if(!name)
            {
                return nullptr;
            }

            std::unique_ptr<ast_node> initialization;

            if(next_is(token_type_t::equal))
            {
                eat(); // equal
                initialization = infix();
            }

            return std::make_unique<variable_declaration_node>(*ty, *name, std::move(initialization));
        }

        boost::optional<type_reference> parser2::type()
        {
            if(generic_type_starts())
            {
                return generic_type();
            }
            else
            {
                return simple_type();
            }
        }

        boost::optional<parser2::token_t> parser2::eat_simple_type()
        {
            const auto token_type = m_token->get_type();

            if (is_builtin_simple_type(token_type) ||
                token_type == token_type_t::identifier)
            {
                return eat();
            }
            else
            {
                raise_error();
                return {};
            }
        }

        bool parser2::is_builtin_simple_type(token_type_t token_type) const
        {
            const auto simple_types = {
                    token_type_t::kw_int,
                    token_type_t::kw_double,
                    token_type_t::kw_bool,
                    token_type_t::kw_string
            };

            return cmsl::contains(builtin_types, token_type);
        }

        boost::optional<type_reference> parser2::simple_type()
        {
            const auto type_token = *m_token;

            if (!eat_simple_type())
            {
                return {};
            }

            return type_reference{type_token, type_token};
        }

        boost::optional<type_reference> parser2::generic_type()
        {
            const auto name_token = *m_token;

            if(!eat_generic_type())
            {
                return nullptr;
            }

            if(!eat(token_type_t::less))
            {
                return nullptr;
            }

            const auto value_type = type();

            if(!value_type)
            {
                return nullptr;
            }

            if(is_at_end())
            {
                raise_error();
                return nullptr;
            }

            const auto closing_greater_token = *m_token;

            if(!eat(token_type_t::greater))
            {
                return nullptr;
            }

            return type_reference{name_token, closing_greater_token};
        }

        bool parser2::generic_type_starts() const
        {
            return current_is_generic_type() && peek() == token_type_t::less;
        }

        parser2::token_type_t parser2::peek(size_t n) const
        {
            if (std::distance(m_token, m_end) <= n)
            {
                return token_type_t::undef;
            }

            return std::next(m_token, n)->get_type();
        }

        bool parser2::current_is_generic_type() const
        {
            const auto generic_types = {
                token_type_t::kw_list
            };

            return cmsl::contains(generic_types, m_token->get_type());
        }

        boost::optional<token_t> parser2::eat_generic_type()
        {
            if(!current_is_generic_type())
            {
                raise_error();
                return false;
            }

            return eat();
        }

        bool parser2::expect_not_at_end()
        {
            if (is_at_end())
            {
                raise_error();
                return false;
            }

            return true;
        }

        boost::optional<token_t> parser2::eat(boost::optional<token_type_t> type)
        {
            if (!expect_not_at_end())
            {
                return {};
            }

            if (type && !current_is(*type))
            {
                raise_error();
                return {};
            }

            *m_token++;
        }

        bool parser2::current_is(token_type_t token_type) const
        {
            if(is_at_end())
            {
                return false;
            }

            return m_token->get_type() == token_type;
        }

        bool parser2::is_at_end() const
        {
            return m_token == m_end;
        }

        bool parser2::next_is(token_type_t token_type) const
        {
            return peek() == token_type;
        }

        std::unique_ptr<ast_node> parser2::infix()
        {
            return std::unique_ptr<ast_node>();
        }
    }
}

