#pragma once

#include <functional>
#include <variant>

namespace cmsl
{
    namespace sema
    {
        class sema_node;
    }

    namespace tools
    {
        struct standalone_expression_context
        {
            std::reference_wrapper<const sema::sema_node> node;
            unsigned place;
        };

        using completion_context_t = std::variant<standalone_expression_context>;
    }
}
