#pragma once

#include <string>
#include <unordered_set>

namespace cmsl
{
    namespace sema
    {
        class sema_node;
        class sema_context;
    }

    namespace tools
    {
        class identifier_names_collector
        {
        public:
            std::unordered_set<std::string> collect(const sema::sema_node& start_node) const;
        };
    }
}
