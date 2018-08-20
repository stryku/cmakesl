#pragma once

#include "common/string.hpp"
#include "exec/stmt/statement.hpp"
#include "exec/instance/instances_holder.hpp"

#include <boost/variant.hpp>

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
        namespace inst
        {
            class instance;
            class instance_factory;
        }

        namespace onp
        {
            struct onp_entry;
            class onp_executor
            {
            private:
                using token_t = lexer::token::token;
                using tokens_container_t = std::vector<token_t>;
                using token_type_t = lexer::token::token_type;
                using stack_entry_t = boost::variant<inst::instance*, const token_t&>;

            public:
                explicit onp_executor(const tokens_container_t& onp_tokens, executor& e, int& result);

                void execute();

            private:
                void execute_function_call(const ast::function_node& fun);
                stack_entry_t get_top_and_pop();
                inst::instance* apply_operator(const stack_entry_t& lhs, token_type_t op, const stack_entry_t& rhs);

            private:
                const tokens_container_t& m_tokens;
                executor& m_exec;
                int& m_result;
                std::stack<stack_entry_t> m_stack;
                inst::instances_holder m_instances;
            };
        }
    }
}
