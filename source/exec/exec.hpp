#pragma once

#include "common/string.hpp"

namespace cmsl
{
    namespace exec
    {
        class executor
        {
        public:
            void execute(cmsl::string_view source);
        };
    }
}
