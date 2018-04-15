#include "common/source_location.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace test
    {
        namespace source_location
        {
            using source_location_t = cmsl::source_location;

            TEST(SourceLocation, New_Zero)
            {
                source_location_t sl;

                EXPECT_THAT(sl.line(), 0u);
                EXPECT_THAT(sl.column(), 0u);
                EXPECT_THAT(sl.absolute(), 0u);
            }
        }
    }
}
