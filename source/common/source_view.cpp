#include "common/source_view.hpp"
#include "source_view.hpp"


namespace cmsl
{
    source_view::source_view(cmsl::string_view source)
        : source_view("<unknown source>", source)
    {}

    source_view::source_view(cmsl::string_view source_path, cmsl::string_view source)
        : m_path{ source_path }
        , m_source{ source }
    {}

    cmsl::string_view source_view::path() const
    {
        return m_path;
    }

    cmsl::string_view source_view::source() const
    {
        return m_source;
    }

    source_view::line_info source_view::line(unsigned line_no) const
    {
        unsigned counter{ 1u };

        auto pos = cmsl::string_view::size_type{ 0u };

        while(counter < line_no)
        {
            pos = m_source.find('\n', pos + 1);
            if(pos == cmsl::string_view::npos)
            {
                return {"", 0};
            }

            ++counter;
        }

        const auto line_end = [pos, source = m_source]
        {
            const auto p = source.find('\n', pos + 1);
            return p != cmsl::string_view::npos ? p : source.size();
        }();

        const auto line_size = line_end - (pos + 1);
        const auto line_begin = std::next(m_source.cbegin(), pos + 1);

        return { cmsl::string_view{ line_begin, line_size }, pos + 1 };
    }

    cmsl::string_view::const_iterator source_view::cbegin() const
    {
        return std::cbegin(m_source);
    }

    cmsl::string_view::const_iterator source_view::cend() const
    {
        return std::cend(m_source);
    }
}
