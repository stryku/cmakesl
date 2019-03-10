#pragma once

#include <string>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class version_value
            {
            public:
                explicit version_value(unsigned major, unsigned minor = 0u, unsigned patch = 0u, unsigned tweak = 0u);

                std::string to_string() const;

                bool operator==(const version_value& rhs) const;
                bool operator!=(const version_value& rhs) const;
                bool operator<(const version_value& rhs) const;
                bool operator<=(const version_value& rhs) const;
                bool operator>(const version_value& rhs) const;
                bool operator>=(const version_value& rhs) const;

            private:
                unsigned m_major{ 0u };
                unsigned m_minor{ 0u };
                unsigned m_patch{ 0u };
                unsigned m_tweak{ 0u };
            };
        }
    }
}
