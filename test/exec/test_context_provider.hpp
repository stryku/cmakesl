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
            explicit test_context_provider()
                    : m_ast_ctx{ std::make_unique<cmsl::ast::builtin_ast_context>() }
            {}

            explicit test_context_provider(std::unique_ptr<cmsl::ast::ast_context> ast_ctx)
                : m_ast_ctx{ std::move(ast_ctx) }
            {}

            const cmsl::ast::ast_context& get_ast_ctx() const override
            {
                return *m_ast_ctx;
            }

            cmsl::exec::execution_context& get_exec_ctx()
            {
                return m_exec_ctx;
            }

            void set_ast_ctx(std::unique_ptr<cmsl::ast::ast_context> ast_ctx)
            {
                m_ast_ctx = std::move(ast_ctx);
            }

        private:
            std::unique_ptr<cmsl::ast::ast_context> m_ast_ctx;
            cmsl::exec::execution_context m_exec_ctx;
        };
    }
}
