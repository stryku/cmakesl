#pragma once

#include "sema/sema_nodes.hpp"
#include "sema/sema_node_visitor.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/expression_evaluation_context.hpp"
#include "exec/context_provider.hpp"
#include "exec/execution_context.hpp"
#include "sema/sema_context.hpp"
#include "exec/function_caller.hpp"
#include "exec/identifiers_context.hpp"
#include "common/assert.hpp"

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
                result = m_ctx.instances.create2(node.value());
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
                result = m_ctx.ids_context.lookup_identifier(node.id().str());
            }

            void visit(const sema::binary_operator_node& node) override
            {
                auto lhs_result = evaluate_child(node.lhs());
                auto rhs_result = evaluate_child(node.rhs());
                const auto operator_string = node.op().str();
                const auto& operator_function = node.operator_function();

                // Todo: use small vector.
                std::vector<inst::instance*> params;
                params.emplace_back(rhs_result);
                result = m_ctx.function_caller.call_member(*lhs_result, operator_function, std::move(params));
            }

            void visit(const sema::function_call_node& node) override
            {
                auto evaluated_params = evaluate_call_parameters(node.param_expressions());
                const auto& function = node.function();
                result = m_ctx.function_caller.call(function, evaluated_params);
            }

            void visit(const sema::member_function_call_node& node) override
            {
                auto lhs_result = evaluate_child(node.lhs());
                auto evaluated_params = evaluate_call_parameters(node.param_expressions());
                const auto& function = node.function();
                result = m_ctx.function_caller.call_member(*lhs_result, function, evaluated_params);
            }

            void visit(const sema::class_member_access_node& node) override
            {
                auto lhs = evaluate_child(node.lhs());
                result = lhs->get_member(node.member_name().str());
            }

            void visit(const sema::return_node& node) override
            {
                result = evaluate_child(node.expression());
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
                                   return evaluate_child(*param_expression);
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

                    default:
                        break;
                }

                CMSL_UNREACHABLE("Unknown operator token");
                return "";
            }

        private:
            expression_evaluation_context& m_ctx;
        };
    }
}
