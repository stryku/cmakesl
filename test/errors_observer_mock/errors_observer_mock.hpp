#pragma once

#include "errors/error.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace errors
    {
        namespace test
        {
            struct errors_observer_mock
            {
                errors_observer_mock();
                ~errors_observer_mock();

                MOCK_METHOD1(notify_error, void(
                        const error&));
            };
        }
    }
}
