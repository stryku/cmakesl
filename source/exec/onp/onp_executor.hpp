#pragma once

#include "common/string.hpp"
#include "exec/stmt/statement.hpp"
#include "exec/instance/instance_value.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/onp/id_access.hpp"
#include "exec/builtin_member_function_caller.hpp"

#include <boost/variant.hpp>

#include <memory>
#include <vector>
#include <stack>

namespace cmsl
{
    namespace ast
    {
        class function;
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
                explicit onp_executor(const tokens_container_t& onp_tokens, execution& e);

                std::unique_ptr<inst::instance> execute();
                inst::instance* execute_function_call(const ast::user_function_node& fun);
                inst::instance* execute_member_function_call(inst::instance* class_instance, cmsl::string_view name);

            private:
                stack_entry_t get_top_and_pop();
                inst::instance* apply_operator(stack_entry_t& lhs, token_type_t op, stack_entry_t& rhs);
                std::vector<inst::instance*> prepare_parameters_for_call(const ast::function& fun);
                inst::instance* get_instance_from_stack_top();

                bool is_function_or_ctor(const token_t& tok) const;

                inst::instance* handle_function_call(const token_t& tok);

            private:
                const tokens_container_t& m_tokens;
                execution& m_exec;
                std::stack<stack_entry_t> m_stack;
                inst::instances_holder m_instances;
                builtin_member_function_caller m_builtin_member_fun_caller;
            };
        }
    }
}
