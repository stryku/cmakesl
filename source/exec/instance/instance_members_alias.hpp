#pragma once

#include "common/string.hpp"

#include <memory>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance;
            using instance_members_t = string_view_map<std::unique_ptr<instance>>;
        }
    }
}
