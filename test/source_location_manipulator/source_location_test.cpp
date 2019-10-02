#include "lexer/source_location_manipulator.hpp"

#include <gmock/gmock.h>

#include <memory>

using namespace testing;

// Todo: change to cmsl::source_location::test
namespace cmsl::test::source_location {
using source_location_manipulator_t = cmsl::lexer::source_location_manipulator;

TEST(SourceLocationManipulator, InitialState__1_Line__1_Column__0_Absolute)
{
  const auto source = "";
  source_location_manipulator_t sl_manip{ source };

  const auto loc = sl_manip.location();

  EXPECT_THAT(loc.line, 1u);
  EXPECT_THAT(loc.column, 1u);
  EXPECT_THAT(loc.absolute, 0u);
}

struct IncrementSourceLocationTestState
{
  std::string source;
  size_t to_increment;
};

template <typename IncrementState = IncrementSourceLocationTestState>
struct IncrementFixture : TestWithParam<IncrementState>
{
  virtual void SetUp()
  {
    const auto& state = TestWithParam<IncrementState>::GetParam();
    sl_manip = std::make_unique<source_location_manipulator_t>(state.source);

    for (auto i = 0u; i < state.to_increment; ++i) {
      sl_manip->consume_char();
    }
  }

  virtual void TearDown() { sl_manip.reset(); }

  std::unique_ptr<source_location_manipulator_t> sl_manip;
};

namespace inc {
struct IncrementsAndExpectedLocationTestState
{
  std::string source;
  size_t to_increment;
  size_t expected_line;
  size_t expected_column;
  size_t expected_absolute;
};

struct Increments
  : public IncrementFixture<IncrementsAndExpectedLocationTestState>
{
};

TEST_P(Increments, ExpectedLocation)
{
  const auto loc = sl_manip->location();

  EXPECT_THAT(loc.line, GetParam().expected_line);
  EXPECT_THAT(loc.column, GetParam().expected_column);
  EXPECT_THAT(loc.absolute, GetParam().expected_absolute);
}

const auto values =
  Values(IncrementsAndExpectedLocationTestState{ "01234", 1u, 1u, 2u, 1u },
         IncrementsAndExpectedLocationTestState{ "01234", 2u, 1u, 3u, 2u },
         IncrementsAndExpectedLocationTestState{
           "01234", 10u, 1u, 5u, 5u }, // Increment more than chars in source
         IncrementsAndExpectedLocationTestState{
           "0\n", 1u, 2u, 1u, 1u }, // Stop at newline -> get next line
         IncrementsAndExpectedLocationTestState{ "0\n1", 2u, 2u, 2u, 2u });
INSTANTIATE_TEST_CASE_P(SourceLocationManipulator, Increments, values);
}

namespace is_at_end {
namespace should_not_reach_end {
struct LessIncrementsThanSourceSize : public IncrementFixture<>
{
};

TEST_P(LessIncrementsThanSourceSize, ShouldNotReachEnd)
{
  EXPECT_THAT(sl_manip->is_at_end(), false);
}

const auto values = Values(IncrementSourceLocationTestState{ "01234", 0u },
                           IncrementSourceLocationTestState{ "01234", 1u },
                           IncrementSourceLocationTestState{ "01234", 4u },
                           IncrementSourceLocationTestState{ "01\n1234", 1u },
                           IncrementSourceLocationTestState{ "01\n1234", 3u });
INSTANTIATE_TEST_CASE_P(SourceLocationManipulator,
                        LessIncrementsThanSourceSize, values);
}

namespace should_reach_end {
struct SourceSizeOrMoreIncrements : public IncrementFixture<>
{
};

TEST_P(SourceSizeOrMoreIncrements, ShouldReachEnd)
{
  EXPECT_THAT(sl_manip->is_at_end(), true);
}

const auto values = Values(IncrementSourceLocationTestState{ "01234", 5u },
                           IncrementSourceLocationTestState{ "01234", 10u });
INSTANTIATE_TEST_CASE_P(SourceLocationManipulator, SourceSizeOrMoreIncrements,
                        values);
}
}

namespace has_next {
struct LessThanSourceSizeMinusOneIncrements : public IncrementFixture<>
{
};

TEST_P(LessThanSourceSizeMinusOneIncrements, HasNext)
{
  EXPECT_THAT(sl_manip->has_next(), true);
}

const auto values = Values(IncrementSourceLocationTestState{ "01234", 0u },
                           IncrementSourceLocationTestState{ "01234", 1u },
                           IncrementSourceLocationTestState{ "01234", 3u },
                           IncrementSourceLocationTestState{ "0\n1234", 1u },
                           IncrementSourceLocationTestState{ "0\n1234", 3u });
INSTANTIATE_TEST_CASE_P(SourceLocationManipulator,
                        LessThanSourceSizeMinusOneIncrements, values);
}

namespace has_not_next {
struct SourceSizeMinusOneOrMoreIncrements : public IncrementFixture<>
{
};

TEST_P(SourceSizeMinusOneOrMoreIncrements, HasNoNext)
{
  EXPECT_THAT(sl_manip->has_next(), false);
}

const auto values = Values(IncrementSourceLocationTestState{ "01234", 4u },
                           IncrementSourceLocationTestState{ "01234", 5u },
                           IncrementSourceLocationTestState{ "01234", 10u });
INSTANTIATE_TEST_CASE_P(SourceLocationManipulator,
                        SourceSizeMinusOneOrMoreIncrements, values);
}
}
