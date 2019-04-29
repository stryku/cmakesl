#pragma once

#include "ast/type_representation.hpp"
#include <optional>

namespace cmsl::ast
{
    struct type_parsing_result
    {
        std::optional<type_representation> ty;
        lexer::token_container_t::const_iterator stopped_at;
    };
}
