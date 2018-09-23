#pragma once

#include "exec/function_caller.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace test
    {
        namespace exec
        {
            class function_caller_mock : public cmsl::exec::function_caller
            {
            public:
                MOCK_METHOD2(call, cmsl::exec::inst::instance*(const cmsl::ast::function&, const std::vector<cmsl::exec::inst::instance*>&));
            };
        }
    }
}
