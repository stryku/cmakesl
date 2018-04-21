#pragma once

#include "common/source_location.hpp"
#include "common/string.hpp"

namespace cmsl
{
    namespace lexer
    {
        class source_location_manipulator
        {
        public:
            explicit source_location_manipulator(cmsl::string_view source);

            source_location location() const;

            cmsl::string_view::value_type current_char() const;
            cmsl::string_view::value_type next_char() const;

            bool is_at_end() const;
            bool has_next() const;

            void consume_char();

        private:
            cmsl::string_view::const_iterator current_it() const;

        private:
            const cmsl::string_view m_source;
            source_location m_source_loc;
        };
    }
}
