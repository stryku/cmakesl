#include "ast/type_parser.hpp"

#include "ast/type_representation.hpp"
#include "common/algorithm.hpp"
#include "ast/parse_errors_observer.hpp"
#include "ast/type_parsing_result.hpp"
#include "type_parser.hpp"


namespace cmsl::ast
{
    type_parser::type_parser(parse_errors_observer &err_observer, parser_utils::token_it current, parser_utils::token_it end)
        : parser_utils{ err_observer, current, end }
        , m_err_observer{ err_observer }
    {}

    type_parsing_result type_parser::type()
    {
        auto got_type = parse_type();
        auto stopped_at = current_iterator();
        return type_parsing_result{ std::move(got_type), stopped_at };
    }

    std::optional<type_representation> type_parser::generic_type()
    {
        const auto name_token = eat_generic_type_token();
        if(!name_token)
        {
            return {};
        }

        const auto less_token = eat(token_type_t::less);
        if(!less_token)
        {
            return {};
        }

        auto value_type = parse_type();
        if(!value_type)
        {
            return {};
        }

        if(!expect_not_at_end())
        {
            return {};
        }

        const auto greater_token = eat(token_type_t::greater);
        if(!greater_token)
        {
            return {};
        }

        token_container_t tokens;
        tokens.emplace_back(*name_token);
        tokens.emplace_back(*less_token);
        tokens.insert(std::end(tokens),
                      std::cbegin(value_type->tokens()), std::cend(value_type->tokens()));
        tokens.emplace_back(*greater_token);

        const auto is_reference = next_is(token_type_t::amp);
        if(is_reference)
        {
            const auto ref_token = eat(token_type_t::amp);
            return type_representation{ tokens,
                                        *ref_token,
                                        { std::move(*value_type) } };
        }

        return type_representation{ tokens, { std::move(*value_type) } };
    }

    bool type_parser::generic_type_starts() const
    {
        return current_is_generic_type() && peek() == token_type_t::less;
    }

    bool type_parser::current_is_generic_type() const
    {
        const auto generic_types = {
                token_type_t::kw_list
        };

        return cmsl::contains(generic_types, curr_type());
    }

    std::optional<type_parser::token_t> type_parser::eat_generic_type_token()
    {
        if(!current_is_generic_type())
        {
            m_err_observer.raise_expected_type(get_token_for_error_report());
            return {};
        }

        return eat();
    }

    std::optional<type_representation> type_parser::simple_type()
    {
        const auto type_token = eat_simple_type_token();
        if (!type_token)
        {
            return {};
        }

        const auto is_reference =current_is(token_type_t::amp);
        if(is_reference)
        {
            const auto ref_token = eat(token_type_t::amp);
            return type_representation{ *type_token,
                                        *ref_token};
        }

        return type_representation{ *type_token };
    }

    std::optional<type_parser::token_t> type_parser::eat_simple_type_token()
    {
        const auto token_type = curr_type();

        if (is_builtin_simple_type(token_type) ||
            token_type == token_type_t::identifier)
        {
            return eat();
        }
        else
        {
            m_err_observer.raise_expected_type(get_token_for_error_report());
            return {};
        }
    }

    bool type_parser::is_builtin_simple_type(token_type_t token_type) const
    {
        const auto simple_types = {
                token_type_t::kw_int,
                token_type_t::kw_double,
                token_type_t::kw_bool,
                token_type_t::kw_string,
                token_type_t::kw_version,
                token_type_t::kw_list,
                token_type_t::kw_library,
                token_type_t::kw_executable
        };

        return cmsl::contains(simple_types, token_type);
    }

    std::optional<type_representation> type_parser::parse_type()
    {
        return generic_type_starts() ? generic_type() : simple_type();;
    }
}