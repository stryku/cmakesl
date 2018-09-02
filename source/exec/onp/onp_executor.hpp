#pragma once

#include "common/string.hpp"
#include "exec/stmt/statement.hpp"
#include "exec/instance/instance_value.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/onp/id_access.hpp"

#include <boost/variant.hpp>

#include <memory>
#include <vector>
#include <stack>

namespace cmsl
{
    namespace ast
    {
        class user_function_node;
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
                using stack_entry_t = boost::variant<inst::instance*, id_access>;

            public:
                explicit onp_executor(const tokens_container_t& onp_tokens, source_executor& e, inst::instance_value_t& result);

                void execute();
                inst::instance* execute_function_call(const ast::user_function_node& fun);
                inst::instance* execute_member_function_call(inst::instance* class_instance, cmsl::string_view name);

            private:
                stack_entry_t get_top_and_pop();
                inst::instance* apply_operator(stack_entry_t& lhs, token_type_t op, stack_entry_t& rhs);
                std::vector<inst::instance*> prepare_parameters_for_call(const ast::user_function_node& fun);
                inst::instance* get_instance_from_stack_top();

            private:
                const tokens_container_t& m_tokens;
                source_executor& m_exec;
                inst::instance_value_t& m_result;
                std::stack<stack_entry_t> m_stack;
                inst::instances_holder m_instances;
            };
        }
    }
}
