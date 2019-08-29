#include "errors_observer_mock/errors_observer_mock.hpp"
#include "errors/errors_observer.hpp"

namespace {
cmsl::errors::test::errors_observer_mock* errors_observer_mock_ptr;
}

namespace cmsl::errors {
namespace test {
errors_observer_mock::errors_observer_mock()
{
  assert(!errors_observer_mock_ptr);
  errors_observer_mock_ptr = this;
}

errors_observer_mock::~errors_observer_mock()
{
  errors_observer_mock_ptr = nullptr;
}
}

void errors_observer::notify_error(const cmsl::errors::error& error)
{
  errors_observer_mock_ptr->notify_error(error);
}
}
