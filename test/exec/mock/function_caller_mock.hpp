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
            private:
                using inst_t = cmsl::exec::inst::instance;

            public:
                MOCK_METHOD2(call, inst_t*(const cmsl::ast::function&, const std::vector<inst_t*>&));
                MOCK_METHOD3(call_member, inst_t*(inst_t&, const cmsl::ast::function&, const std::vector<inst_t*>&));
            };
        }
    }
}
