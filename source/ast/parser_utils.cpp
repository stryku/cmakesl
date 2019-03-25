#include "ast/parser_utils.hpp"

#include "errors/errors_observer.hpp"
#include "errors/error.hpp"

namespace cmsl::ast
{
    parser_utils::parser_utils(errors::errors_observer& err_observer, token_it current, token_it end)
            : m_err_observer{ err_observer }
            , m_current{ current }
            , m_end{ end }
    {}

    void parser_utils::raise_error(lexer::token::token token, std::string message)
    {
        m_err_observer.nofify_error(errors::error{token.src_range(), message});
    }

    bool parser_utils::is_at_end() const
    {
        return m_current == m_end;
    }

    bool parser_utils::expect_not_at_end()
    {
        if (is_at_end())
        {
            // Todo: proper token
            raise_error(lexer::token::token{}, "Unexpected end of source");
            return false;
        }

        return true;
    }

    parser_utils::token_type_t parser_utils::peek(size_t n) const
    {
        if (std::distance(m_current, m_end) <= n)
        {
            return token_type_t::undef;
        }

        return std::next(m_current, n)->get_type();
    }

    boost::optional<parser_utils::token_t> parser_utils::eat(boost::optional<token_type_t> type)
    {
        if (!expect_not_at_end())
        {
            return {};
        }

        if (type && !current_is(*type))
        {
            raise_error(*m_current, "Expected " + to_string(*type));
            return {};
        }

        const auto t = *m_current;
        ++m_current;
        return t;
    }

    const parser_utils::token_t& parser_utils::current() const
    {
        return *m_current;
    }

    parser_utils::token_type_t parser_utils::curr_type() const
    {
        return is_at_end() ? token_type_t::undef : m_current->get_type();
    }

    bool parser_utils::next_is(token_type_t token_type) const
    {
        return peek() == token_type;
    }

    bool parser_utils::current_is(token_type_t token_type) const
    {
        if(is_at_end())
        {
            return false;
        }

        return curr_type() == token_type;
    }
}
