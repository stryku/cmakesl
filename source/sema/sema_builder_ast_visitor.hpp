#pragma once

#include "ast/ast_node_visitor.hpp"
#include "ast/ast_context.hpp"
#include "ast/type_operator_support_check.hpp"
#include "ast/common_type_finder.hpp"
#include "ast/class_type.hpp"
#include "ast/while_node.hpp"
#include "ast/conditional_node.hpp"
#include "ast/if_else_node.hpp"
#include "ast/translation_unit_node.hpp"


#include "ast/variable_declaration_node.hpp"
#include "ast/infix_nodes.hpp"
#include "ast/return_node.hpp"
#include "ast/class_node.hpp"

#include "common/algorithm.hpp"

#include "errors/error.hpp"
#include "errors/errors_observer.hpp"

#include "sema/sema_nodes.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/sema_function_builder.hpp"
#include "sema/user_sema_function.hpp"
#include "sema/factories.hpp"
#include "sema/sema_context.hpp"
#include "sema/sema_type.hpp"
#include "sema/type_builder.hpp"

namespace cmsl
{
    namespace sema
    {
        class sema_builder_ast_visitor : public ast::ast_node_visitor
        {
        private:
            using param_expressions_t = std::vector<std::unique_ptr<expression_node>>;

        public:
            explicit sema_builder_ast_visitor(sema_context_interface& ctx,
                                              errors::errors_observer& errs,
                                              identifiers_context& ids_context,
                                              sema_type_factory& type_factory,
                                              sema_function_factory& function_factory,
                                              sema_context_factory& context_factory)
                : m_ctx{ ctx }
                , m_errors_observer{ errs }
                , m_ids_context{ ids_context }
                , m_function_factory{ function_factory }
                , m_context_factory{ context_factory }
                , m_type_factory{ type_factory }
            {}

            void visit(const ast::block_node& node) override
            {
                auto ig = ids_guard();

                std::vector<std::unique_ptr<sema_node>> nodes;

                for(auto n : node.get_expressions())
                {
                    auto child = visit_child(*n);
                    if(!child)
                    {
                        return;
                    }

                    nodes.emplace_back(std::move(child));
                }

                m_result_node = std::make_unique<block_node>(std::move(nodes));
            }

            void visit(const ast::class_node2& node) override
            {
                const auto name = node.get_name();
                if(const auto found_type = m_ctx.find_type_in_this_scope(name.str()))
                {
                    // Todo: type redefinition
                    raise_error(name, "Type redefinition");
                    return;
                }

                auto class_ids_guard = ids_guard();

                auto& class_context = m_context_factory.create_class(&m_ctx);
//                auto created_class_sema_ctx = std::make_unique<sema_context>(&m_ctx);
//                auto class_sema_ctx = created_class_sema_ctx.get();

                auto members = collect_class_members_and_add_functions_to_ctx(node, class_context);

                if(!members)
                {
                    return;
                }

                // We move created ast ctx ownership but it will live for the whole program lifetime, so it is safe to use class_ast_ctx raw pointer.
                // Todo: ast shouldn't store ast ctx somewhere else?
                auto& class_type = m_type_factory.create(class_context, name, std::move(members->info));
//                auto created_class_type = std::make_unique<sema_type>(class_context, name, std::move(members->info));
//                auto class_type = created_class_type.get();

                // Same as with ast context. class_type raw pointer will be valid. We move it to context to make this class findable as a regular type (so e.g. inside this class methods, the type will appear as a regular type).
                m_ctx.add_type(class_type);

                std::vector<std::unique_ptr<function_node>> functions;

                for(auto function_declaration : members->functions)
                {
                    auto body = visit_child_node<block_node>(function_declaration.body_to_visit, class_context);
                    if(!body)
                    {
                        return;
                    }

                    function_declaration.fun->set_body(*body);
                    functions.emplace_back(std::make_unique<function_node>(*function_declaration.fun, std::move(body)));
                }

                m_result_node = std::make_unique<class_node>(name, std::move(members->declarations),
                        std::move(functions));
            }

            void visit(const ast::conditional_node& node) override
            {
                auto ig = ids_guard();

                auto condition = visit_child_expr(node.get_condition());
                if(!condition)
                {
                    return;
                }

                auto body = visit_child_node<block_node>(node.get_block());
                if(!body)
                {
                    return;
                }

                m_result_node = std::make_unique<conditional_node>(std::move(condition), std::move(body));
            }

            void visit(const ast::if_else_node& node) override
            {
                std::vector<std::unique_ptr<conditional_node>> ifs;

                for(const auto& cond_node : node.get_ifs())
                {
                    auto cond = visit_child_node<conditional_node>(*cond_node);
                    if(!cond)
                    {
                        return;
                    }

                    ifs.emplace_back(std::move(cond));
                }

                std::unique_ptr<block_node> else_body;

                if(auto else_node = node.get_else())
                {
                    auto ig = ids_guard();

                    else_body = visit_child_node<block_node>(*else_node);
                    if(!else_body)
                    {
                        return;
                    }
                }

                m_result_node = std::make_unique<if_else_node>(std::move(ifs), std::move(else_body));
            }

            void visit(const ast::binary_operator_node& node) override
            {
                ast::type_operator_support_check check;

                auto lhs = visit_child_expr(node.get_lhs());
                if(!lhs)
                {
                    return;
                }

                // Todo: handle operators like ++ and ++(int)
                const auto op = node.get_operator();
                auto operator_function = lhs->type().find_member_function(op.str());
                if(!operator_function)
                {
                    // Todo: lhs's type doesn't support such operator
                    raise_error(op, lhs->type().name().str().to_string() + " doesn't support operator " + op.str().to_string());
                    return;
                }

                auto rhs = visit_child_expr(node.get_rhs());
                if(!rhs)
                {
                    return;
                }

                if(lhs->type() != rhs->type())
                {
                    // Todo: can not apply operator to different types
                    raise_error(op, "Can not apply operator to with different operand types");
                    return;
                }

                m_result_node = std::make_unique<binary_operator_node>(std::move(lhs),
                                                                       node.get_operator(),
                                                                       *operator_function,
                                                                       std::move(rhs),
                                                                       lhs->type());
            }

            void visit(const ast::class_member_access_node& node) override
            {
                auto lhs = visit_child_expr(node.get_lhs());
                if(!lhs)
                {
                    return;
                }

                const auto& lhs_type = lhs->type();
                const auto member_name = node.get_member_name();
                auto member_info = lhs_type.find_member(member_name.str());
                if(!member_info)
                {
                    // Todo: type does not have such member.
                    raise_error(member_name, lhs_type.name().str().to_string() + " does not have member " + member_name.str().to_string());
                    return;
                }

                m_result_node = std::make_unique<class_member_access_node>(std::move(lhs), *member_info);
            }

            void visit(const ast::function_call_node& node) override
            {
                const auto name = node.get_name();
                const auto found_fun = m_ctx.find_function(name.str());
                if(!found_fun)
                {
                    // Todo: can't find function with such name.
                    raise_error(name, name.str().to_string() + " function not found");
                    return;
                }

                auto params = get_function_call_params(found_fun->signature(), node.get_param_nodes());
                if(!params)
                {
                    return;
                }

                if(m_ctx.type() == sema_context_interface::context_type::namespace_)
                {
                    m_result_node = std::make_unique<function_call_node>(*found_fun, std::move(*params));
                }
                else
                {
                    m_result_node = std::make_unique<implicit_member_function_call_node>(*found_fun, std::move(*params));
                }
            }

            void visit(const ast::member_function_call_node& node) override
            {
                const auto name = node.get_name();
                auto lhs = visit_child_expr(node.get_lhs());
                if(!lhs)
                {
                    return;
                }

                auto function = lhs->type().find_member_function(name.str());
                if(!function)
                {
                    // Todo: type has no such function
                    raise_error(name, name.str().to_string() + " member function not found");
                    return;
                }

                auto params = get_function_call_params(function->signature(), node.get_param_nodes());
                if(!params)
                {
                    return;
                }

                m_result_node = std::make_unique<member_function_call_node>(std::move(lhs), *function, std::move(*params));
            }

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
                    raise_error(id_token, id_token.str().to_string() + " identifier not found");
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

            void visit(const ast::translation_unit_node& node) override
            {
                auto ig = ids_guard();
                std::vector<std::unique_ptr<sema_node>> nodes;

                for(auto n : node.get_nodes())
                {
                    auto visited_node = visit_child(*n);
                    if(!visited_node)
                    {
                        return;
                    }

                    nodes.emplace_back(std::move(visited_node));
                }

                m_result_node = std::make_unique<translation_unit_node>(std::move(nodes));
            }

            void visit(const ast::user_function_node2& node) override
            {
                const auto return_type_reference = node.get_return_type_reference();
                auto return_type = m_ctx.find_type(return_type_reference.to_string());
                if(!return_type)
                {
                    // Todo: unknown return type
                    raise_error(return_type_reference.begin, "Unknown return type");
                    return;
                }

                // Todo: parameters need to be in the same ids context as function body.
                auto params_ids_guard = ids_guard();

                using param_decl_t = parameter_declaration;
                std::vector<param_decl_t> params;

                for(const auto& param_decl : node.get_param_declarations())
                {
                    auto param_type = m_ctx.find_type(param_decl.ty.to_string());
                    if(!param_type)
                    {
                        //Todo: unknown parameter type
                        raise_error(param_decl.ty.begin, "Unknown parameter type");
                        return;
                    }

                    params.emplace_back(param_decl_t{*param_type, param_decl.name});
                    m_ids_context.register_identifier(param_decl.name, param_type);
                }

                function_signature signature{
                        node.get_name(),
                        std::move(params)
                };
                auto& function = m_function_factory.create_user( m_ctx, *return_type, std::move(signature) );

                // Add function (without a body yet) to context, so it will be visible inside function body in case of a recursive call.
                m_ctx.add_function(function);

                auto block = visit_child_node<block_node>(node.get_body());
                if(!block)
                {
                    return;
                }

                // And set the body.
                function.set_body(*block);

                m_result_node = std::make_unique<function_node>(function, std::move(block));
            }

            void visit(const ast::variable_declaration_node& node) override
            {
                const auto type_reference = node.get_type_reference();
                const auto type = m_ctx.find_type(type_reference.to_string());

                // Todo: handle generic types
                if(!type)
                {
                    // Todo: type not found
                    raise_error(type_reference.begin, "Unknown variable type");
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

                    if(initialization->type() != *type)
                    {
                        // Todo: Init does not have same type as declared.
                        // Todo: introduce auto
                        raise_error(initialization->type().name(), "Initialization and declared variable type does not match");
                        return;
                    }
                }

                m_ids_context.register_identifier(node.get_name(), type);
                m_result_node = std::make_unique<variable_declaration_node>(*type, node.get_name(), std::move(initialization));
            }

            void visit(const ast::while_node& node) override
            {
                auto conditional = visit_child_node<conditional_node>(node.get_node());
                if(!conditional)
                {
                    return;
                }

                m_result_node = std::make_unique<while_node>(std::move(conditional));
            }

        public:
            std::unique_ptr<sema_node> m_result_node;

        private:
            template <typename T>
            std::unique_ptr<T> to_node(std::unique_ptr<sema_node> node) const
            {
                return std::unique_ptr<T>(dynamic_cast<T*>(node.release()));
            }

            std::unique_ptr<expression_node> to_expression(std::unique_ptr<sema_node> node) const
            {
                return std::unique_ptr<expression_node>(dynamic_cast<expression_node*>(node.release()));
            }

            void raise_error(const lexer::token::token token, const std::string& message)
            {
                m_errors_observer.nofify_error({token.src_range(), message}); // todo: get error via parameter
            }

            sema_builder_ast_visitor clone() const
            {
                return clone(m_ctx);
            }

            sema_builder_ast_visitor clone(sema_context_interface& ctx_to_visit) const
            {
                return sema_builder_ast_visitor{ ctx_to_visit, m_errors_observer, m_ids_context, m_type_factory, m_function_factory, m_context_factory };
            }

            std::unique_ptr<expression_node> visit_child_expr(const ast::ast_node& node)
            {
                return to_node<expression_node>(visit_child(node));
            }

            template <typename T>
            std::unique_ptr<T> visit_child_node(const ast::ast_node& node)
            {
                return to_node<T>(visit_child(node));
            }

            template <typename T>
            std::unique_ptr<T> visit_child_node(const ast::ast_node& node, sema_context_interface& ctx_to_visit)
            {
                return to_node<T>(visit_child(node, ctx_to_visit));
            }

            std::unique_ptr<sema_node> visit_child(const ast::ast_node& node)
            {
                auto v = clone();
                node.visit(v);
                return std::move(v.m_result_node);
            }

            std::unique_ptr<sema_node> visit_child(const ast::ast_node& node, sema_context_interface& ctx_to_visit)
            {
                auto v = clone(ctx_to_visit);
                node.visit(v);
                return std::move(v.m_result_node);
            }

            boost::optional<param_expressions_t> get_function_call_params(const function_signature& signature, const ast::call_node::params_t& passed_params)
            {
                const auto& param_declarations = signature.params;

                if(param_declarations.size() != passed_params.size())
                {
                    // Todo passed wrong number of parameters
                    raise_error(signature.name, "Expected " + std::to_string(param_declarations.size()) + " parameters, got " + std::to_string(passed_params.size()));
                    return {};
                }

                std::vector<std::unique_ptr<expression_node>> params;

                for(auto i = 0u; i < param_declarations.size(); ++i)
                {
                    auto param = visit_child_expr(*passed_params[i]);
                    if(!param)
                    {
                        return {};
                    }

                    if(param_declarations[i].ty != param->type())
                    {
                        //Todo passed param type mismatch
                        // Todo: pass param tokens range instead of empty token
                        raise_error(lexer::token::token{}, "Passed parameter does not match, expected " + param_declarations[i].ty.name().str().to_string()
                        + " got " + param->type().name().str().to_string());
                        return {};
                    }

                    params.emplace_back(std::move(param));
                }

                return params;
            }

            struct ids_ctx_guard
            {
                explicit ids_ctx_guard(identifiers_context& ids_context)
                    : m_ids_ctx{ ids_context }
                {
                    m_ids_ctx.enter_ctx();
                }

                ~ids_ctx_guard()
                {
                    m_ids_ctx.leave_ctx();
                }

                identifiers_context& m_ids_ctx;
            };

            ids_ctx_guard ids_guard()
            {
                return ids_ctx_guard{ m_ids_context };
            }

            struct function_declaration
            {
                user_sema_function* fun{ nullptr };
                const ast::block_node& body_to_visit;
            };

            struct class_members
            {
                std::vector<member_info> info;
                std::vector<std::unique_ptr<variable_declaration_node>> declarations;
                std::vector<function_declaration> functions;
            };

            boost::optional<function_declaration> get_function_declaration_and_add_to_ctx(const ast::user_function_node2& node,
                    sema_context& ctx)
            {
                const auto return_type_reference = node.get_return_type_reference();
                auto return_type = ctx.find_type(return_type_reference.to_string());
                if(!return_type)
                {
                    // Todo: unknown return type
                    raise_error(return_type_reference.begin, return_type_reference.to_string().to_string() + " unknown return type");
                    return {};
                }

                // Todo: parameters need to be in the same ids context as function body.
                auto params_ids_guard = ids_guard();

                std::vector<parameter_declaration> params;

                for(const auto& param_decl : node.get_param_declarations())
                {
                    auto param_type = ctx.find_type(param_decl.ty.to_string());
                    if(!param_type)
                    {
                        //Todo: unknown parameter type
                        raise_error(param_decl.ty.begin, param_decl.ty.to_string().to_string() + " unknown parameter type");
                        return {};
                    }

                    params.emplace_back(parameter_declaration{*param_type, param_decl.name});
                    m_ids_context.register_identifier(param_decl.name, param_type);
                }

                function_signature signature{
                        node.get_name(),
                        std::move(params)
                };
                auto& function = m_function_factory.create_user( ctx, *return_type, std::move(signature) );
                ctx.add_function(function);

                return function_declaration{
                        &function,
                        node.get_body()
                };
            }

            boost::optional<class_members> collect_class_members_and_add_functions_to_ctx(const ast::class_node2& node,
                    sema_context& class_context)
            {
                class_members members;

                for(auto n : node.get_nodes())
                {
                    if (auto variable_decl = dynamic_cast<const ast::variable_declaration_node *>(n))
                    {
                        auto member = visit_child_node<variable_declaration_node>(*variable_decl);
                        if (!member)
                        {
                            return {};
                        }

                        members.info.emplace_back(member_info{member->name(), member->type()});
                        members.declarations.emplace_back(std::move(member));
                    }
                    else if(auto fun_node = dynamic_cast<const ast::user_function_node2*>(n))
                    {
                        auto function_declaration = get_function_declaration_and_add_to_ctx(*fun_node, class_context);
                        if(!function_declaration)
                        {
                            return {};
                        }

                        members.functions.emplace_back(*function_declaration);
                    }
                }

                return members;
            }

        private:
            sema_context_interface& m_ctx;
            errors::errors_observer& m_errors_observer;
            identifiers_context& m_ids_context;
            sema_type_factory& m_type_factory;
            sema_function_factory& m_function_factory;
            sema_context_factory& m_context_factory;
        };
    }
}
