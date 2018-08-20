#pragma once

#include "lexer/token/token_type.hpp"

namespace cmsl
{
    namespace lexer
    {
        namespace token
        {
            class token;
        }
    }

    namespace exec
    {
        class execution_context;

        namespace inst
        {
            class instance;
            class instance_factory;
            class instances_holder;
        }

        namespace onp
        {
            class operator_visitor
            {
            private:
                using instance_t = exec::inst::instance;
                using token_t = lexer::token::token;
                using token_type_t = lexer::token::token_type;
                using execution_context_t = exec::execution_context;
                using instance_factory_t = exec::inst::instance_factory;
                using instances_holder_t = exec::inst::instances_holder;

            public:
                explicit operator_visitor(token_type_t op,
                                          execution_context_t& exec_ctx,
                                          instances_holder_t& instances);

                instance_t* operator()(instance_t* lhs, instance_t* rhs);
                instance_t* operator() (instance_t* lhs, const token_t& token);
                instance_t* operator()(const token_t& lhs, const token_t& rhs);
                instance_t* operator()(const token_t& lhs, instance_t* rhs);

            private:
                token_type_t m_operator;
                execution_context_t& m_exec_ctx;
                instances_holder_t& m_instances;
            };
        }
    }
}


