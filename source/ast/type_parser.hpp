#pragma once

#include "ast/parser_utils.hpp"

namespace cmsl::ast
{
    class type_representation;
    struct type_parsing_result;

    class type_parser : public parser_utils
    {
    public:
        explicit type_parser(parse_errors_observer &err_observer, token_it current, token_it end);

        type_parsing_result type();

    private:
        boost::optional<token_t> eat_generic_type_token();

        boost::optional<type_representation> parse_type();
        boost::optional<type_representation> generic_type();
        boost::optional<type_representation> simple_type();
        bool generic_type_starts() const;

        bool current_is_generic_type() const;
        bool is_builtin_simple_type(token_type_t token_type) const;
        boost::optional<token_t> eat_simple_type_token();

    private:
        parse_errors_observer &m_err_observer;
    };
}
