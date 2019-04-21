#pragma once

#include "common/string.hpp"

namespace cmsl
{
    struct source_location
    {
        unsigned line{ 1u };
        unsigned column{ 1u };
        unsigned absolute{ 0u };

        bool operator<(const source_location& rhs) const
        {
            return absolute < rhs.absolute;
        }

        bool operator==(const source_location& rhs) const
        {
            return absolute == rhs.absolute;
        }

        bool operator<=(const source_location& rhs) const
        {
            return *this < rhs || *this == rhs;
        }
    };

    struct source_range
    {
        source_location begin;
        source_location end;

        constexpr auto size() const
        {
            return end.absolute - begin.absolute;
        }
    };
}
