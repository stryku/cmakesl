#include "exec/instance/version_value.hpp"
#include "version_value.hpp"

#include <algorithm>
#include <tuple>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            version_value::version_value(unsigned major, unsigned minor, unsigned patch, unsigned tweak)
                : m_major{ major }
                , m_minor{ minor }
                , m_patch{ patch }
                , m_tweak{ tweak }
            {}

            std::string version_value::to_string() const
            {
                // Todo: Skip trailing zeroes. E.g. 1.2.0.0, should be "1.2".
                return std::to_string(m_major) + '.'
                       + std::to_string(m_minor) + '.'
                       + std::to_string(m_patch) + '.'
                       + std::to_string(m_tweak);
            }

            bool version_value::operator<(const version_value &rhs) const
            {
                const auto this_parts = { m_major, m_minor, m_patch, m_tweak };
                const auto rhs_parts = { rhs.m_major, rhs.m_minor, rhs.m_patch, rhs.m_tweak };

                return std::lexicographical_compare(std::cbegin(this_parts), std::cend(this_parts),
                                                    std::cbegin(rhs_parts), std::cend(rhs_parts));
            }

            bool version_value::operator==(const version_value &rhs) const
            {
                return std::tie(m_major, m_minor, m_patch, m_tweak) ==
                std::tie(rhs.m_major, rhs.m_minor, rhs.m_patch, rhs.m_tweak);
            }

            bool version_value::operator!=(const version_value &rhs) const
            {
                return !(*this == rhs);
            }

            bool version_value::operator<=(const version_value &rhs) const
            {
                return *this == rhs || *this < rhs;
            }

            bool version_value::operator>(const version_value &rhs) const
            {
                return !(*this <= rhs);
            }

            bool version_value::operator>=(const version_value &rhs) const
            {
                return !(*this < rhs);
            }
        }
    }
}
