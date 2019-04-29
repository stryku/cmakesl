#pragma once

#include "common/int_alias.hpp"

#include <string>

namespace cmsl::exec::inst
{
            class version_value
            {
            public:
                explicit version_value(int_t major, int_t minor = 0u, int_t patch = 0u, int_t tweak = 0u);

                // Avoid major and minor macros from sysmacros.h
                int_t major_() const;
                int_t minor_() const;
                int_t patch() const;
                int_t tweak() const;

                std::string to_string() const;

                bool operator==(const version_value& rhs) const;
                bool operator!=(const version_value& rhs) const;
                bool operator<(const version_value& rhs) const;
                bool operator<=(const version_value& rhs) const;
                bool operator>(const version_value& rhs) const;
                bool operator>=(const version_value& rhs) const;

            private:
                int_t m_major{ 0u };
                int_t m_minor{ 0u };
                int_t m_patch{ 0u };
                int_t m_tweak{ 0u };
            };
}
