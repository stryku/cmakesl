#include "lexer/source_location_manipulator.hpp"

namespace cmsl::lexer
{
        source_location_manipulator::source_location_manipulator(cmsl::string_view source)
            : m_source{ source }
        {}

        source_location source_location_manipulator::location() const
        {
            return m_source_loc;
        }

        cmsl::string_view::value_type source_location_manipulator::current_char() const
        {
            return *current_it();
        }

        cmsl::string_view::value_type source_location_manipulator::next_char() const
        {
            return *std::next(current_it());
        }

        bool source_location_manipulator::is_at_end() const
        {
            return current_it() == m_source.end();
        }

        bool source_location_manipulator::has_next() const
        {
            if (is_at_end())
            {
                return false;
            }

            const auto next_it = std::next(current_it());
            return next_it != m_source.end();
        }

        cmsl::string_view::const_iterator source_location_manipulator::current_it() const
        {
            return std::next(m_source.begin(), m_source_loc.absolute);
        }

        void source_location_manipulator::consume_char()
        {
            if (is_at_end())
            {
                return;
            }

            ++m_source_loc.absolute;

            if (current_char() == '\n')
            {
                ++m_source_loc.line;
                m_source_loc.column = 1u;
            }
            else
            {
                ++m_source_loc.column;
            }
        }
}
