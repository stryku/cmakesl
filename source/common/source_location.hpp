#pragma once

namespace cmsl
{
    class source_location
    {
    public:
        size_t line() const;
        size_t column() const;
        size_t absolute() const;
    };
}
