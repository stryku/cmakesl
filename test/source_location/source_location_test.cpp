#include "common/source_location.hpp"

#include <gmock/gmock.h>

using namespace testing;

namespace cmsl
{
    namespace test
    {
        namespace source_location
        {
            using source_location_t = cmsl::source_location;

            TEST(SourceLocation, InitialState__1_Line__1_Column__0_Absolute)
            {
                const auto source = "";
                source_location_t sl{ source };

                EXPECT_THAT(sl.line(), 1u);
                EXPECT_THAT(sl.column(), 1u);
                EXPECT_THAT(sl.absolute(), 0u);
            }

            TEST(SourceLocation, IncrementEmpty_GetInitialState)
            {
                const auto source = "";
                source_location_t sl{ source };

                ++sl;

                EXPECT_THAT(sl.line(), 1u);
                EXPECT_THAT(sl.column(), 1u);
                EXPECT_THAT(sl.absolute(), 0u);
            }

            namespace inc
            {
                struct IncrementSourceLocationTestState
                {
                    std::string source;
                    size_t to_increment;
                    size_t expected_line;
                    size_t expected_column;
                    size_t expected_absolute;
                };

                struct Increment : public Test, WithParamInterface<IncrementSourceLocationTestState>
                {};

                TEST_P(Increment, Increment)
                {
                    const auto state = GetParam();
                    source_location_t sl{ state.source };

                    for (auto i = 0u; i < state.to_increment; ++i)
                    {
                        ++sl;
                    }

                    EXPECT_THAT(sl.line(), state.expected_line);
                    EXPECT_THAT(sl.column(), state.expected_column);
                    EXPECT_THAT(sl.absolute(), state.expected_absolute);
                }

                const auto values = Values(
                    IncrementSourceLocationTestState{ "01234", 1u, 1u, 2u, 1u },
                    IncrementSourceLocationTestState{ "01234", 2u, 1u, 3u, 2u },
                    IncrementSourceLocationTestState{ "01234", 10u, 1u, 6u, 5u }, // Increment more than chars in source
                    IncrementSourceLocationTestState{ "0\n", 1u, 2u, 1u, 1u }, // Stop at newline -> get next line
                    IncrementSourceLocationTestState{ "0\n1", 2u, 2u, 2u, 2u }
                    );
                INSTANTIATE_TEST_CASE_P(SourceLocation, Increment, values);
            }

            namespace is_at_end
            {
                struct IsAtEndSourceLocationTestState
                {
                    std::string source;
                    size_t to_increment;
                    bool expected_is_at_end;
                };

                struct IsAtEnd : public Test, WithParamInterface<IsAtEndSourceLocationTestState>
                {};

                TEST_P(IsAtEnd, IsAtEnd)
                {
                    const auto state = GetParam();
                    source_location_t sl{ state.source };

                    for (auto i = 0u; i < state.to_increment; ++i)
                    {
                        ++sl;
                    }

                    EXPECT_THAT(sl.is_at_end(), state.expected_is_at_end);
                }

                const auto values = Values(
                    IsAtEndSourceLocationTestState{ "01234", 0u, false },
                    IsAtEndSourceLocationTestState{ "01234", 1u, false },
                    IsAtEndSourceLocationTestState{ "01234", 4u, false },
                    IsAtEndSourceLocationTestState{ "0\n1234", 1u, false },
                    IsAtEndSourceLocationTestState{ "0\n1234", 3u, false },
                    IsAtEndSourceLocationTestState{ "01234", 5u, true },
                    IsAtEndSourceLocationTestState{ "01234", 10u, true }
                );
                INSTANTIATE_TEST_CASE_P(SourceLocation, IsAtEnd, values);
            }

            namespace has_next
            {
                struct HasNextSourceLocationTestState
                {
                    std::string source;
                    size_t to_increment;
                    bool expected_has_next;
                };

                struct IsAtEnd : public Test, WithParamInterface<HasNextSourceLocationTestState>
                {};

                TEST_P(IsAtEnd, IsAtEnd)
                {
                    const auto state = GetParam();
                    source_location_t sl{ state.source };

                    for (auto i = 0u; i < state.to_increment; ++i)
                    {
                        ++sl;
                    }

                    EXPECT_THAT(sl.has_next(), state.expected_has_next);
                }

                const auto values = Values(
                    HasNextSourceLocationTestState{ "01234", 0u, true },
                    HasNextSourceLocationTestState{ "01234", 1u, true },
                    HasNextSourceLocationTestState{ "01234", 3u, true },
                    HasNextSourceLocationTestState{ "0\n1234", 1u, true },
                    HasNextSourceLocationTestState{ "0\n1234", 3u, true },
                    HasNextSourceLocationTestState{ "01234", 4u, false },
                    HasNextSourceLocationTestState{ "01234", 5u, false },
                    HasNextSourceLocationTestState{ "01234", 10u, false }
                );
                INSTANTIATE_TEST_CASE_P(SourceLocation, IsAtEnd, values);
            }
        }
    }
}
