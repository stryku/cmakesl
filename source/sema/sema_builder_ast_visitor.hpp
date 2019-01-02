#pragma once

#include "ast/ast_node_visitor.hpp"
#include "ast/ast_context.hpp"

#include "ast/variable_declaration_node.hpp"

#include "errors/errors_observer.hpp"

#include "sema/sema_nodes.hpp"

namespace cmsl
{
    namespace sema
    {
        class sema_builder_ast_visitor : public ast::ast_node_visitor
        {
        public:
            explicit sema_builder_ast_visitor(ast::ast_context& ctx, errors::errors_observer& errs)
                : m_ctx{ ctx }
                , m_errors_observer{ errs }
            {}

            void visit(const ast::block_node& node) override {}
            void visit(const ast::class_node2& node) override {}
            void visit(const ast::conditional_node& node) override {}
            void visit(const ast::if_else_node& node) override {}
            void visit(const ast::binary_operator_node& node) override {}
            void visit(const ast::class_member_access_node& node) override {}
            void visit(const ast::function_call_node& node) override {}
            void visit(const ast::member_function_call_node& node) override {}
            void visit(const ast::bool_value_node& node) override {}
            void visit(const ast::int_value_node& node) override {}
            void visit(const ast::double_value_node& node) override {}
            void visit(const ast::string_value_node& node) override {}
            void visit(const ast::id_node& node) override {}
            void visit(const ast::return_node& node) override {}
            void visit(const ast::translation_unit_node& node) override {}
            void visit(const ast::user_function_node2& node) override {}

            void visit(const ast::variable_declaration_node& node) override
            {
                const auto type = m_ctx.find_type(node.get_type_reference().to_string());

                if(!type)
                {
                    raise_error();
                    return;
                }

                std::unique_ptr<sema_node> initialization;
                if(auto init_node = node.get_initialization())
                {
                    auto v = clone();
                    init_node->visit(v);
                    if(!v.m_result_node)
                    {
                        return;
                    }

                    initialization = std::move(v.m_result_node);
                }

                m_result_node = std::make_unique<variable_declaration_node>(*type, node.get_name(), std::move(initialization));
            }

            void visit(const ast::while_node& node) override {}

        private:
            void raise_error()
            {
                m_errors_observer.nofify_error({}); // todo: get error via parameter
            }

            sema_builder_ast_visitor clone() const
            {
                return sema_builder_ast_visitor{ m_ctx, m_errors_observer };
            }

        private:
            ast::ast_context& m_ctx;
            errors::errors_observer& m_errors_observer;
            std::unique_ptr<sema_node> m_result_node;
        };
    }
}
