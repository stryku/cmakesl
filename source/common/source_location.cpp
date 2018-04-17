#include "common/source_location.hpp"

#include <algorithm>

namespace cmsl
{
    source_location::source_location(cmsl::string_view source) :
        m_source{ source }
    {}

    size_t source_location::line() const
    {
        return m_line;
    }

    size_t source_location::column() const
    {
        return m_column;
    }

    size_t source_location::absolute() const
    {
        return m_absolute;
    }

    bool source_location::is_at_end() const
    {
        return current_it() == m_source.end();
    }

    source_location& source_location::operator++()
    {
        if (is_at_end())
        {
            return *this;
        }

        ++m_absolute;

        if (current_char() == '\n')
        {
            ++m_line;
            m_column = 1u;
        }
        else
        {
            ++m_column;
        }

        return *this;
    }

    cmsl::string_view::value_type source_location::current_char() const
    {
        return m_source[m_absolute];
    }

    cmsl::string_view::const_iterator source_location::current_it() const
    {
        return std::next(m_source.begin(), m_absolute);
    }

    bool source_location::has_next() const
    {
        if (is_at_end())
        {
            return false;
        }

        const auto next_it = std::next(current_it());
        return next_it != m_source.end();
    }
}
