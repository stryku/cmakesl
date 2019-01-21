#pragma once

#include "sema/sema_nodes.hpp"
#include "sema/sema_node_visitor.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/expression_evaluation_context.hpp"
#include "exec/context_provider.hpp"
#include "exec/execution_context.hpp"

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

            void visit(const sema::binary_operator_node& node) override {}

            void visit(const sema::function_call_node& node) override {}
            void visit(const sema::member_function_call_node& node) override {}
            void visit(const sema::class_member_access_node& node) override {}

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

            expression_evaluation_visitor clone()
            {
                return expression_evaluation_visitor{ m_ctx };
            }

        private:
            expression_evaluation_context& m_ctx;
        };
    }
}
