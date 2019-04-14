#pragma once

#include "common/string.hpp"

namespace cmsl
{
    struct source_location
    {
        size_t line{ 1u };
        size_t column{ 1u };
        size_t absolute{ 0u };

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
