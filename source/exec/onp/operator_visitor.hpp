#pragma once

#include "lexer/token/token_type.hpp"
#include "common/enum_class_utils.hpp"
#include "exec/instance/instance_value.hpp"

#include <functional>
#include <unordered_map>

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
            class id_access;
            class onp_executor;

            class operator_visitor
            {
            private:
                using instance_t = exec::inst::instance;
                using token_t = lexer::token::token;
                using token_type_t = lexer::token::token_type;
                using execution_context_t = exec::execution_context;
                using instance_factory_t = exec::inst::instance_factory;
                using instances_holder_t = exec::inst::instances_holder;

                using arith_operator_handler_t = std::function<instance_t*(instance_t*, instance_t*)>;
                using arith_operators_handlers_t = enum_unordered_map<token_type_t, arith_operator_handler_t>;

            public:
                explicit operator_visitor(token_type_t op,
                                          execution_context_t& exec_ctx,
                                          instances_holder_t& instances,
                                          onp_executor& function_caller);

                instance_t* operator()(instance_t* lhs, instance_t* rhs);
                instance_t* operator()(instance_t* lhs, id_access& rhs);
                instance_t* operator()(id_access& lhs, id_access& rhs);
                instance_t* operator()(id_access& lhs, instance_t* rhs);

            private:
                instance_t* get_instance(id_access& token);
                instance_t* handle_dot_operator(instance_t* lhs, id_access& rhs);
                instance_t* handle_arith_operator(instance_t* lhs, token_type_t  op, instance_t* rhs);

                arith_operators_handlers_t get_arith_operators_handlers();
                arith_operator_handler_t get_arith_operator_handler(token_type_t op);
                arith_operator_handler_t get_compound_assignment_handler(token_type_t op);

                // Todo remove static
                static inst::instance_value_t apply_operator_visitor(instance_t* lhs, token_type_t  op, instance_t* rhs);


            private:
                token_type_t m_operator;
                execution_context_t& m_exec_ctx;
                instances_holder_t& m_instances;
                onp_executor& m_function_caller;
                arith_operators_handlers_t m_arith_op_handlers;
            };
        }
    }
}


