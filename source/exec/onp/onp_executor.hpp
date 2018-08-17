#pragma once

#include "exec/stmt/statement.hpp"

#include <memory>
#include <vector>
#include <stack>

namespace cmsl
{
    namespace ast
    {
        class function_node;
    }

    namespace lexer
    {
        namespace token
        {
            class token;
            enum class token_type;
        }
    }

    namespace exec
    {
        class instance;
        class instance_factory;

        namespace onp
        {
            struct onp_entry;

            class onp_executor
            {
            private:
                using tokens_container_t = std::vector<lexer::token::token>;
                using token_type_t = lexer::token::token_type;

            public:
                explicit onp_executor(const std::vector<onp_entry>& onp_tokens, executor& e, int& result);

                void execute();

            private:
                void execute_function_call(const ast::function_node& fun);
                std::unique_ptr<instance> get_top_and_pop();
                instance_factory get_factory();
                std::unique_ptr<instance> apply_operator(instance* lhs, token_type_t op, instance* rhs);
                std::unique_ptr<instance> apply_dot_operator(instance* lhs, instance* rhs);

            private:
                const std::vector<onp_entry>& m_tokens;
                executor& m_exec;
                int& m_result;
                std::stack<std::unique_ptr<instance>> m_stack;
            };
        }
    }
}
