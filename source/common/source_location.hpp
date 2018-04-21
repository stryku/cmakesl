#pragma once

#include "common/string.hpp"

namespace cmsl
{
    class source_location
    {
    public:
        explicit source_location();
        explicit source_location(cmsl::string_view source);

        size_t line() const;
        size_t column() const;
        size_t absolute() const;

        source_location& operator++();

        cmsl::string_view::value_type current_char() const;
        cmsl::string_view::value_type next_char() const;

        bool is_at_end() const;
        bool has_next() const;

    private:
        cmsl::string_view::const_iterator current_it() const;

        size_t m_line{ 1u };
        size_t m_column{ 1u };
        size_t m_absolute{ 0u };
        cmsl::string_view m_source;
    };

    struct source_range
    {
        source_location begin;
        source_location end;
    };
}
