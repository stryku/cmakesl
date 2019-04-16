#pragma once

#include <string>
#include <vector>

namespace cmsl
{
    namespace sema
    {
        class sema_node;
    }

    namespace tools
    {
        class type_names_collector
        {
        public:
            std::vector<std::string> collect(const sema::sema_node& start_node) const;
        };
    }
}
