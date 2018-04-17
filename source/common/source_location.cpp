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
        return std::next(m_source.begin(), m_absolute) == m_source.end();
    }

    source_location& source_location::operator++()
    {
        if (is_at_end())
        {
            return *this;
        }
    }
}
