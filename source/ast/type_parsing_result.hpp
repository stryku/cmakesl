#pragma once

#include "ast/type_representation.hpp"
#include <boost/optional.hpp>

namespace cmsl::ast
{
    struct type_parsing_result
    {
        boost::optional<type_representation> ty;
        lexer::token::token_container_t::const_iterator stopped_at;
    };
}
