#include "errors/errors_observer.hpp"
#include "errors_observer_mock/errors_observer_mock.hpp"

namespace
{
    cmsl::test::errors_observer::errors_observer_mock* errors_observer_mock_ptr;
}

namespace cmsl
{
    namespace test
    {
        namespace errors_observer
        {
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
    }

    namespace errors
    {
        void errors_observer::nofify_error(const cmsl::errors::error& error)
        {
            errors_observer_mock_ptr->notify_error(error);
        }
    }
}
