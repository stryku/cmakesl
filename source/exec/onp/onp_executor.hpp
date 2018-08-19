#pragma once

#include "common/string.hpp"
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
                explicit onp_executor(const tokens_container_t& onp_tokens, executor& e, int& result);

                void execute();

            private:
                void execute_function_call(const ast::function_node& fun);
                instance* get_top_and_pop();
                instance_factory get_factory();
                instance* apply_operator(instance* lhs, token_type_t op, instance* rhs);
                instance* apply_dot_operator(instance* lhs, instance* rhs);

                instance* create_instance(int value);

            private:
                const tokens_container_t& m_tokens;
                executor& m_exec;
                int& m_result;
                std::stack<instance*> m_stack;
                std::vector<std::unique_ptr<instance>> m_instances;
            };
        }
    }
}
