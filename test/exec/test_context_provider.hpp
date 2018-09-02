#pragma once

#include "ast/builtin_ast_context.hpp"
#include "exec/execution_context.hpp"
#include "exec/context_provider.hpp"

namespace cmsl
{
    namespace test
    {
        class test_context_provider : public cmsl::exec::context_provider
        {
        public:
            const cmsl::ast::ast_context& get_ast_ctx() const override
            {
                return m_ast_ctx;
            }

            cmsl::exec::execution_context& get_exec_ctx()
            {
                return m_exec_ctx;
            }

        private:
            cmsl::ast::builtin_ast_context m_ast_ctx;
            cmsl::exec::execution_context m_exec_ctx;
        };
    }
}
