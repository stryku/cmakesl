#pragma once

#include "common/string.hpp"

namespace cmsl
{
    namespace exec
    {
        class source_executor
        {
        public:
            int execute(cmsl::string_view source);
        };
    }
}
