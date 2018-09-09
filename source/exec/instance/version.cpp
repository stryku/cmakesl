#include "exec/instance/version.hpp"

#include <ostream>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            bool version::operator==(const version& other) const
            {
                return std::tie(major, minor, patch, tweak) == std::tie(other.major,
                                                                        other.minor,
                                                                        other.patch,
                                                                        other.tweak);
            }

            bool version::operator!=(const version& other) const
            {
                return !(*this == other);
            }
        }
    }
}

namespace std
{
    std::ostream &operator<<(std::ostream &out, const cmsl::exec::inst::version& v)
    {
        out << v.major << '.' << v.minor << '.' << v.patch << '.' << v.tweak;
        return out;
    }
}
