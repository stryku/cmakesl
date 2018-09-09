#pragma once

#include "exec/instance/int_alias.hpp"

#include <tuple>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            struct version
            {
                int_t major{};
                int_t minor{};
                int_t patch{};
                int_t tweak{};

                bool operator==(const version& other) const;
                bool operator!=(const version& other) const;
            };
        }
    }
}

namespace std
{
    std::ostream &operator<<(std::ostream &out, const cmsl::exec::inst::version& v);
}
