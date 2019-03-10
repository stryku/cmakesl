#pragma once

#include "common/source_location.hpp"
#include <string>

namespace cmsl
{
    namespace errors
    {
        struct error
        {
            source_range range;
            std::string message;
        };
    }
}
