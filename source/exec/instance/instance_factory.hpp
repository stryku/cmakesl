#pragma once

#include "common/string.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class ast_context;
        class type;
    }

    namespace exec
    {
        class execution_context;

        namespace inst
        {
            class instance;

            class instance_factory
            {
            public:
                explicit instance_factory(const ast::ast_context &ast_ctx, execution_context &exec_ctx);

                std::unique_ptr<instance> create(int value) const;

                std::unique_ptr<instance> create(cmsl::string_view name) const;

                std::unique_ptr<instance> create(cmsl::string_view name, int value) const;

            private:
                const ast::type &get_int_type() const;

            private:
                const ast::ast_context &m_ast_ctx;
                execution_context &m_exec_ctx;
            };
        }
    }
}
