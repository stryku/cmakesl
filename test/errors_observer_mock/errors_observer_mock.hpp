#pragma once

#include "errors/error.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace test
    {
        namespace errors_observer
        {
            struct errors_observer_mock
            {
                errors_observer_mock();
                ~errors_observer_mock();

                MOCK_METHOD1(notify_error, void(const cmsl::errors::error&));
            };
        }
    }
}
