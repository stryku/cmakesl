#pragma once

#include "sema/sema_nodes.hpp"
#include "sema/sema_node_visitor.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/expression_evaluation_context.hpp"
#include "exec/context_provider.hpp"
#include "exec/execution_context.hpp"
#include "exec/value_operations_factory.hpp"

namespace cmsl
{
    namespace exec
    {
        class expression_evaluation_visitor : public sema::empty_sema_node_visitor
        {
        public:
            explicit expression_evaluation_visitor(expression_evaluation_context& ctx)
                : m_ctx{ ctx }
            {}

            void visit(const sema::bool_value_node& node) override
            {
                result = m_ctx.instances.create(node.value());
            }

            void visit(const sema::int_value_node& node) override
            {
                result = m_ctx.instances.create(node.value());
            }

            void visit(const sema::double_value_node& node) override
            {
                result = m_ctx.instances.create(node.value());
            }

            void visit(const sema::string_value_node& node) override
            {
                result = m_ctx.instances.create(node.value().to_string());
            }

            void visit(const sema::id_node& node) override
            {
                auto& exec_ctx = m_ctx.ctx_provider.get_exec_ctx();
                result = exec_ctx.get_variable(node.id().str());
            }

            void visit(const sema::binary_operator_node& node) override
            {
                auto lhs = evaluate_child(node.lhs());
                auto rhs = evaluate_child(node.rhs());

                auto operations = m_ctx.operations_factory.create(lhs->get_type());
                const auto operator_string = operator_to_str(node.op());

                value_operations::parameters_t params;
                params.emplace_back(rhs->get_value());

                auto operation_result = operations->execute_operation(operator_string, lhs->get_value_ref(), params);
                result = m_ctx.instances.create(std::move(operation_result));
            }

            void visit(const sema::function_call_node& node) override
            {
                auto evaluated_params = evaluate_call_parameters(node.param_expressions());


            }

            void visit(const sema::member_function_call_node& node) override {}
            void visit(const sema::class_member_access_node& node) override
            {
                auto lhs = evaluate_child(node.lhs());
                result = lhs->get_member(node.member_name().str());
            }

        public:
            inst::instance* result;

        private:
            template <typename T>
            inst::instance* evaluate_child(const T& child)
            {
                auto c = clone();
                child.visit(c);
                return c.result;
            }

            std::vector<inst::instance*> evaluate_call_parameters(const sema::call_node::param_expressions_t& params)
            {
                std::vector<inst::instance*> evaluated_params;

                std::transform(std::cbegin(params), std::cend(params),
                               std::back_inserter(evaluated_params),
                               [this](const auto& param_expression)
                               {
                                   return evaluate_child(param_expression);
                               });

                return evaluated_params;
            }

            expression_evaluation_visitor clone()
            {
                return expression_evaluation_visitor{ m_ctx };
            }

            std::string operator_to_str(lexer::token::token op)
            {
                switch(op.get_type())
                {
                    case lexer::token::token_type::equal:
                    case lexer::token::token_type::equalequal:
                    case lexer::token::token_type::minus:
                    case lexer::token::token_type::minusminus:
                    case lexer::token::token_type::minusequal:
                    case lexer::token::token_type::plus:
                    case lexer::token::token_type::plusplus:
                    case lexer::token::token_type::plusequal:
                    case lexer::token::token_type::amp:
                    case lexer::token::token_type::ampamp:
                    case lexer::token::token_type::ampequal:
                    case lexer::token::token_type::pipe:
                    case lexer::token::token_type::pipepipe:
                    case lexer::token::token_type::pipeequal:
                    case lexer::token::token_type::slash:
                    case lexer::token::token_type::slashequal:
                    case lexer::token::token_type::star:
                    case lexer::token::token_type::starequal:
                    case lexer::token::token_type::percent:
                    case lexer::token::token_type::percentequal:
                    case lexer::token::token_type::exclaimequal:
                    case lexer::token::token_type::xor_:
                    case lexer::token::token_type::xorequal:
                    case lexer::token::token_type::less:
                    case lexer::token::token_type::lessequal:
                    case lexer::token::token_type::greater:
                    case lexer::token::token_type::greaterequal:
                    {
                        return op.str().to_string();
                    }
                }

                CMSL_UNREACHABLE("Unknown operator token");
                return "";
            }

        private:
            expression_evaluation_context& m_ctx;
        };
    }
}
