#pragma once

#include "exec/function_caller.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test
        {
            class function_caller_mock : public function_caller
            {
            private:
                using inst_t = inst::instance;

            public:
                MOCK_METHOD2(call, inst_t*(
                        const ast::function&, const std::vector<inst_t*>&));
                MOCK_METHOD3(call_member, inst_t *(inst_t
                        &,
                        const ast::function&, const std::vector<inst_t*>&));
            };

            class function_caller2_mock : public function_caller2
            {
            private:
                using inst_t = inst::instance;

            public:
                MOCK_METHOD3(call, std::unique_ptr<inst_t>(const sema::sema_function&, const std::vector<inst_t*>&, inst::instances_holder_interface&));
                MOCK_METHOD4(call_member, std::unique_ptr<inst_t>(inst_t&,const sema::sema_function&, const std::vector<inst_t*>&, inst::instances_holder_interface&));
            };
}
