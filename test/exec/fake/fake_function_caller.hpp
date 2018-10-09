#pragma once

#include "exec/function_caller.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace test
        {
            class fake_function_caller : public function_caller
            {
            private:
                using inst_t = inst::instance;

            public:
                inst::instance* call(const ast::function&, const std::vector<inst::instance*>&) override
                {
                    return nullptr;
                }

                inst::instance* call_member(inst::instance&,
                                            const ast::function&,
                                            const std::vector<inst::instance*>&) override
                {
                    return nullptr;
                }
            };
        }
    }
}
