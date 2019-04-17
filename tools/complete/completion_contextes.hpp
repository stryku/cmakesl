#pragma once

#include <functional>
#include <variant>

namespace cmsl
{
    namespace sema
    {
        class sema_node;
        class translation_unit_node;
    }

    namespace tools
    {
        struct standalone_expression_context
        {
            std::reference_wrapper<const sema::sema_node> node;
            unsigned place;
        };

        struct top_level_declaration_context
        {
            std::reference_wrapper<const sema::translation_unit_node> node;
            unsigned place;
        };

        struct could_not_find_context{};

        using completion_context_t = std::variant<could_not_find_context,
                                                  standalone_expression_context,
                                                  top_level_declaration_context>;
    }
}
