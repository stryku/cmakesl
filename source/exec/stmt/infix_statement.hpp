#pragma once

#include "exec/instance/instance_value.hpp"
#include "exec/stmt/statement.hpp"

#include <vector>

namespace cmsl
{
    namespace ast
    {
        class infix_node;
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
        namespace onp
        {
            struct onp_entry;
        }

        namespace stmt
        {
            class infix_statement : public statement
            {
            private:
                using tokens_container_t = std::vector<lexer::token::token>;
                using token_type_t = lexer::token::token_type;

            public:
                explicit infix_statement(const ast::infix_node& node, inst::instance_value_t & result);

                virtual void execute(executor& e) override;

            private:
                tokens_container_t to_onp(executor& e) const;

            private:
                const ast::infix_node& m_node;
                inst::instance_value_t& m_result;
            };
        }
    }
}
