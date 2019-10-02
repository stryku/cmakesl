#include "errors/errors_observer.hpp"
#include "errors/error.hpp"
#include "test/mock/cmake_facade_mock.hpp"

#include <gmock/gmock.h>
#include <sstream>

namespace cmsl::errors::test {
using ::testing::Ne;
using ::testing::_;

TEST(ErrorsObserverTest, WithoutStreamAndFacade_NotifyError_DoesntFail)
{
  errors_observer observer;
  error err{};
  EXPECT_NO_FATAL_FAILURE(observer.notify_error(err));
}

TEST(ErrorsObserverTest, WithStream_NotifyError_ForwardsErrorToStream)
{
  std::ostringstream oss;
  errors_observer observer{ oss };
  error err{};
  err.message = "foo";

  observer.notify_error(err);

  const auto found = oss.str().find("error: foo");
  EXPECT_THAT(found, Ne(std::string::npos));
}

TEST(ErrorsObserverTest, WithFacade_NotifyError_ForwardsErrorToFacade)
{
  exec::test::cmake_facade_mock facade_mock;
  errors_observer observer{ &facade_mock };
  error err{};
  EXPECT_CALL(facade_mock, error(_));

  observer.notify_error(err);
}
}
