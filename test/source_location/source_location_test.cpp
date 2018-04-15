#include "common/source_location.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace test
    {
        namespace source_location
        {
            using source_location_t = cmsl::source_location;

            TEST(SourceLocation, InitialState__1_Line__1_Column__0_Absolute)
            {
                source_location_t sl;

                EXPECT_THAT(sl.line(), 1u);
                EXPECT_THAT(sl.column(), 1u);
                EXPECT_THAT(sl.absolute(), 0u);
            }

            TEST(SourceLocation, IncrementEmpty_InitialState)
            {
                source_location_t sl;

                ++sl;

                EXPECT_THAT(sl.line(), 1u);
                EXPECT_THAT(sl.column(), 1u);
                EXPECT_THAT(sl.absolute(), 0u);
            }
        }
    }
}
