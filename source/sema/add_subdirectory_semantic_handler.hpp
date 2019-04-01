#pragma once

#include <memory>
#include <vector>

namespace cmsl::sema
{
    class expression_node;
    class sema_function;

    class add_subdirectory_semantic_handler
    {
    public:
        virtual const sema_function* handle(const std::vector<std::unique_ptr<expression_node>>& params) = 0;
    };
}
