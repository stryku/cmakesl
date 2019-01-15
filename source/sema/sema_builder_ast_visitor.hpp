#pragma once

#include "ast/ast_node_visitor.hpp"
#include "ast/ast_context.hpp"
#include "ast/type_operator_support_check.hpp"
#include "ast/common_type_finder.hpp"


#include "ast/variable_declaration_node.hpp"
#include "ast/infix_nodes.hpp"
#include "ast/return_node.hpp"

#include "common/algorithm.hpp"

#include "errors/error.hpp"
#include "errors/errors_observer.hpp"

#include "sema/sema_nodes.hpp"
#include "sema/identifiers_context.hpp"

namespace cmsl
{
    namespace sema
    {


        class sema_builder_ast_visitor : public ast::ast_node_visitor
        {
        public:
            explicit sema_builder_ast_visitor(ast::ast_context& ctx, errors::errors_observer& errs, identifiers_context& ids_context)
                : m_ctx{ ctx }
                , m_errors_observer{ errs }
                , m_ids_context{ ids_context }
            {}

            void visit(const ast::block_node& node) override {}
            void visit(const ast::class_node2& node) override {}
            void visit(const ast::conditional_node& node) override {}
            void visit(const ast::if_else_node& node) override {}

            void visit(const ast::binary_operator_node& node) override
            {
                const auto op = node.get_operator().get_type();

                ast::type_operator_support_check check;

                auto lhs = visit_child_expr(node.get_lhs());
                if(!lhs)
                {
                    return;
                }

                if(!check.type_supports_operator(lhs->type().get_kind(), op))
                {
                    // Todo: lhs type doesn't support this operator
                    raise_error();
                    return;
                }

                auto rhs = visit_child_expr(node.get_rhs());
                if(!rhs)
                {
                    return;
                }

                if(!check.type_supports_operator(rhs->type().get_kind(), op))
                {
                    // Todo: lhs type doesn't support this operator
                    raise_error();
                    return;
                }

                const ast::type* common_type = &lhs->type();
                if(lhs->type() != rhs->type())
                {
                    common_type = ast::common_type_finder{}.find_common_type(lhs->type(), rhs->type());
                    if(common_type == nullptr)
                    {
                        // Todo: can't apply operator to these types
                        raise_error();
                        return;
                    }
                }

                m_result_node = std::make_unique<binary_operator_node>(std::move(lhs), node.get_operator(), std::move(rhs), *common_type);
            }

            void visit(const ast::class_member_access_node& node) override {}
            void visit(const ast::function_call_node& node) override
            {
                const auto name = node.get_name();
                const auto found_fun = m_ctx.find_function(name.str());
                if(!found_fun)
                {
                    // Todo function can't find function with such name.
                    raise_error();
                    return;
                }

                const auto& param_declarations = found_fun->get_params_declarations();
                const auto& passed_params = node.get_param_nodes();

                if(param_declarations.size() != passed_params.size())
                {
                    // Todo passed wrong number of parameters
                    raise_error();
                    return;
                }

                std::vector<std::unique_ptr<expression_node>> params;

                for(auto i = 0u; i < param_declarations.size(); ++i)
                {
                    auto param = visit_child_expr(*passed_params[i]);
                    if(!param)
                    {
                        return;
                    }

                    if(*param_declarations[i].param_type != param->type())
                    {
                        //Todo passed param type mismatch
                        raise_error();
                        return;
                    }

                    params.emplace_back(std::move(param));
                }

                m_result_node = std::make_unique<function_call_node>(found_fun->get_type(), name, std::move(params));
            }

            void visit(const ast::member_function_call_node& node) override {}

            void visit(const ast::bool_value_node& node) override
            {
                const auto& type = *m_ctx.find_type("bool");
                const auto value = node.get_token().str() == "true";
                m_result_node = std::make_unique<bool_value_node>(type, value);
            }

            void visit(const ast::int_value_node& node) override
            {
                const auto& type = *m_ctx.find_type("int");
                const auto token = node.get_token();
                char* end;
                const auto value = std::strtol(token.str().data(), &end, 10); // todo: handle hex etc
                m_result_node = std::make_unique<int_value_node>(type, value);
            }

            void visit(const ast::double_value_node& node) override
            {
                const auto& type = *m_ctx.find_type("double");
                const auto token = node.get_token();
                char* end;
                const auto value = std::strtod(token.str().data(), &end); // todo: handle hex etc
                m_result_node = std::make_unique<double_value_node>(type, value);
            }

            void visit(const ast::string_value_node& node) override
            {
                const auto& type = *m_ctx.find_type("string");
                m_result_node = std::make_unique<string_value_node>(type, node.get_token().str());
            }

            void visit(const ast::id_node& node) override
            {
                const auto id_token = node.get_identifier();
                const auto type = m_ids_context.type_of(node.get_identifier().str());

                if(!type)
                {
                    // Todo: identifier not found
                    raise_error();
                    return;
                }

                m_result_node = std::make_unique<id_node>(*type, id_token);
            }

            void visit(const ast::return_node& node) override
            {
                auto v = clone();
                node.get_expression().visit(v);
                if(!v.m_result_node)
                {
                    return;
                }

                auto expr = to_expression(std::move(v.m_result_node));
                m_result_node = std::make_unique<return_node>(std::move(expr));
            }

            void visit(const ast::translation_unit_node& node) override {}
            void visit(const ast::user_function_node2& node) override {}

            void visit(const ast::variable_declaration_node& node) override
            {
                const auto type = m_ctx.find_type(node.get_type_reference().to_string());

                // Todo: handle generic types
                if(!type)
                {
                    // Todo: type not found
                    raise_error();
                    return;
                }

                std::unique_ptr<expression_node> initialization;
                if(auto init_node = node.get_initialization())
                {
                    initialization = visit_child_expr(*init_node);
                    if(!initialization)
                    {
                        return;
                    }

                    // Todo: detect implicit conversion and raise a warning
                    if(!ast::type_conversion_check{}.can_convert(initialization->type().get_kind(), type->get_kind()))
                    {
                        // Todo cannot convert from init to declared type
                        raise_error();
                        return;
                    }
                }

                m_ids_context.register_identifier(node.get_name(), type);
                m_result_node = std::make_unique<variable_declaration_node>(*type, node.get_name(), std::move(initialization));
            }

            void visit(const ast::while_node& node) override {}

            std::unique_ptr<sema_node> m_result_node;

        private:
            std::unique_ptr<expression_node> to_expression(std::unique_ptr<sema_node> node) const
            {
                return std::unique_ptr<expression_node>(dynamic_cast<expression_node*>(node.release()));
            }

            void raise_error()
            {
                m_errors_observer.nofify_error({}); // todo: get error via parameter
            }

            sema_builder_ast_visitor clone() const
            {
                return sema_builder_ast_visitor{ m_ctx, m_errors_observer, m_ids_context };
            }

            std::unique_ptr<expression_node> visit_child_expr(const ast::ast_node& node)
            {
                return to_expression(visit_child(node));
            }

            std::unique_ptr<sema_node> visit_child(const ast::ast_node& node)
            {
                auto v = clone();
                node.visit(v);
                return std::move(v.m_result_node);
            }

        private:
            ast::ast_context& m_ctx;
            errors::errors_observer& m_errors_observer;
            identifiers_context& m_ids_context;
        };
    }
}
