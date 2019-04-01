#include "sema/sema_builder_ast_visitor.hpp"

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
#include "sema/overload_resolution.hpp"
#include "common/assert.hpp"
#include "sema/builtin_types_finder.hpp"
#include "sema/variable_initialization_checker.hpp"
#include "sema/add_subdirectory_semantic_handler.hpp"


namespace cmsl
{
    namespace sema
    {
        sema_builder_ast_visitor::ids_ctx_guard::ids_ctx_guard(identifiers_context& ids_context)
                : m_ids_ctx{ ids_context }
        {
            m_ids_ctx.enter_ctx();
        }

        sema_builder_ast_visitor::ids_ctx_guard::~ids_ctx_guard()
        {
            m_ids_ctx.leave_ctx();
        }

        sema_builder_ast_visitor::sema_builder_ast_visitor(sema_context_interface& generic_types_context,
                                                           sema_context_interface& ctx,
                                                           errors::errors_observer& errs,
                                                           identifiers_context& ids_context,
                                                           sema_type_factory& type_factory,
                                                           sema_function_factory& function_factory,
                                                           sema_context_factory& context_factory,
                                                           add_subdirectory_semantic_handler& add_subdirectory_handler,
                                                           sema_function* currently_parsing_function)
                : m_generic_types_context{ generic_types_context }
                , m_ctx{ ctx }
                , m_errors_observer{ errs }
                , m_ids_context{ ids_context }
                , m_function_factory{ function_factory }
                , m_context_factory{ context_factory }
                , m_type_factory{ type_factory }
                , m_add_subdirectory_handler{ add_subdirectory_handler }
                , m_currently_parsed_function{ currently_parsing_function }
        {}

        void sema_builder_ast_visitor::visit(const ast::block_node& node)
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

        void sema_builder_ast_visitor::visit(const ast::class_node2& node)
        {
            const auto name = node.get_name();
            const auto class_name_representation = ast::type_representation{ name };
            if(const auto found_type = m_ctx.find_type_in_this_scope(class_name_representation))
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
            auto& class_type = m_type_factory.create(class_context, class_name_representation, std::move(members->info));
            auto& class_type_reference = m_type_factory.create_reference(class_type);
//                auto created_class_type = std::make_unique<sema_type>(class_context, name, std::move(members->info));
//                auto class_type = created_class_type.get();

            // Same as with ast context. class_type raw pointer will be valid. We move it to context to make this class findable as a regular type (so e.g. inside this class methods, the type will appear as a regular type).
            m_ctx.add_type(class_type);
            m_ctx.add_type(class_type_reference);

            std::vector<std::unique_ptr<function_node>> functions;

            for(auto function_declaration : members->functions)
            {
                m_currently_parsed_function = function_declaration.fun;
                auto body = visit_child_node<block_node>(function_declaration.body_to_visit, class_context);
                m_currently_parsed_function = nullptr;
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

        void sema_builder_ast_visitor::visit(const ast::conditional_node& node)
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

        void sema_builder_ast_visitor::visit(const ast::if_else_node& node)
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

        void sema_builder_ast_visitor::visit(const ast::binary_operator_node& node)
        {
            ast::type_operator_support_check check;

            auto lhs = visit_child_expr(node.get_lhs());
            if(!lhs)
            {
                return;
            }

            // Todo: handle operators like ++ and ++(int)
            const auto op = node.get_operator();
            auto lookup_result = lhs->type().find_member_function(op);
            if(lookup_result.empty())
            {
                // Todo: lhs's type doesn't support such operator
                raise_error(op, lhs->type().name().to_string() + " doesn't support operator " + op.str().to_string());
                return;
            }

            auto rhs = visit_child_expr(node.get_rhs());
            if(!rhs)
            {
                return;
            }

            overload_resolution over_resolution{ m_errors_observer, op };
            const auto chosen_function = over_resolution.choose(lookup_result, *rhs);
            if(!chosen_function)
            {
                return;
            }

            m_result_node = std::make_unique<binary_operator_node>(std::move(lhs),
                                                                   node.get_operator(),
                                                                   *chosen_function,
                                                                   std::move(rhs),
                                                                   chosen_function->return_type());
        }

        void sema_builder_ast_visitor::visit(const ast::class_member_access_node& node)
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
                raise_error(member_name, lhs_type.name().to_string() + " does not have member " + member_name.str().to_string());
                return;
            }

            m_result_node = std::make_unique<class_member_access_node>(std::move(lhs), *member_info);
        }

        const sema_function *sema_builder_ast_visitor::get_function_to_call(lexer::token::token name,
                                                                            const param_expressions_t& params) const
        {
            // add_subdirectory() builtin function is special. At high level,
            // it is suppose to execute script from the given directory.
            // Here, in sema, we need to parse this script, and get pointer to its main
            // function. If it's not found, error is already reported and we should not proceed.
            if(name.str() == "add_subdirectory")
            {
                if(params.empty())
                {
                    // Todo: Error, no dir name provided
                    return nullptr;
                }

                const auto casted = dynamic_cast<const string_value_node*>(params[0].get());
                if(!casted)
                {
                    // Todo: Error, only string literal is allowed.
                    return nullptr;
                }

                const auto name = casted->value();

                std::vector<const expression_node*> params_but_name;
                std::transform(std::next(std::cbegin(params)), std::cend(params),
                        std::back_inserter(params_but_name),
                        [](const auto& param)
                       {
                            return param.get();
                       });
                return m_add_subdirectory_handler.handle_add_subdirectory(name, params_but_name);
            }
            else
            {
                const auto function_lookup_result = m_ctx.find_function(name);
                overload_resolution over_resolution{ m_errors_observer, name };
                return over_resolution.choose(function_lookup_result, params);
            }
        }

        void sema_builder_ast_visitor::visit(const ast::function_call_node& node)
        {
            auto params = get_function_call_params(node.get_param_nodes());
            if(!params)
            {
                return;
            }

            const auto chosen_function = get_function_to_call(node.get_name(), *params);
            if(!chosen_function)
            {
                return;
            }

            // Convert call parameter nodes if need, e.g. to a cast_to_reference_node, if function accepts a reference.
            params = convert_params_to_cast_nodes_if_need(chosen_function->signature(),
                                                          std::move(*params));

            switch (chosen_function->context().type())
            {
                case sema_context_interface::context_type::namespace_:
                {
                    m_result_node = std::make_unique<function_call_node>(*chosen_function, std::move(*params));
                } break;
                case sema_context_interface::context_type::class_:
                {
                    const auto is_constructor = chosen_function->signature().name.str() == chosen_function->return_type().name().to_string();
                    if(is_constructor)
                    {
                        m_result_node = std::make_unique<constructor_call_node>(chosen_function->return_type(),
                                                                                *chosen_function,
                                                                                std::move(*params));
                    }
                    else
                    {
                        m_result_node = std::make_unique<implicit_member_function_call_node>(*chosen_function,
                                                                                             std::move(*params));
                    }
                } break;

                default:
                    CMSL_UNREACHABLE("Unknown context type");
            }
        }

        void sema_builder_ast_visitor::visit(const ast::member_function_call_node& node)
        {
            const auto name = node.get_name();
            auto lhs = visit_child_expr(node.get_lhs());
            if(!lhs)
            {
                return;
            }

            auto params = get_function_call_params(node.get_param_nodes());
            if(!params)
            {
                return;
            }

            const auto member_functions = lhs->type().find_member_function(name);
            overload_resolution over_resolution{ m_errors_observer, node.get_name() };
            const auto chosen_function = over_resolution.choose(member_functions, *params);
            if(!chosen_function)
            {
                return;
            }

            // Convert call parameter nodes if need, e.g. to a cast_to_reference_node, if function accepts a reference.
            params = convert_params_to_cast_nodes_if_need(chosen_function->signature(),
                                                          std::move(*params));

            m_result_node = std::make_unique<member_function_call_node>(std::move(lhs), *chosen_function, std::move(*params));
        }

        void sema_builder_ast_visitor::visit(const ast::bool_value_node& node)
        {
            const auto& type = builtin_types_finder{ m_ctx }.find_bool();
            const auto value = node.get_token().str() == "true";
            m_result_node = std::make_unique<bool_value_node>(type, value);
        }

        void sema_builder_ast_visitor::visit(const ast::int_value_node& node)
        {
            const auto& type = builtin_types_finder{ m_ctx }.find_int();
            const auto token = node.get_token();
            char* end;
            const auto value = std::strtol(token.str().data(), &end, 10); // todo: handle hex etc
            m_result_node = std::make_unique<int_value_node>(type, value);
        }

        void sema_builder_ast_visitor::visit(const ast::double_value_node& node)
        {
            // Todo: figure out better way to find builtin types
            const auto& type = builtin_types_finder{ m_ctx }.find_double();
            const auto token = node.get_token();
            char* end;
            const auto value = std::strtod(token.str().data(), &end); // todo: handle hex etc
            m_result_node = std::make_unique<double_value_node>(type, value);
        }

        void sema_builder_ast_visitor::visit(const ast::string_value_node& node)
        {
            // Todo: figure out better way to find builtin types
            const auto& type = builtin_types_finder{ m_ctx }.find_string();
            // At this point node contains string value including "". We need to get rid of them.

            const auto node_string = node.get_token().str();
            const auto string_without_quotation_marks = cmsl::string_view{ std::next(node_string.begin()),
                                                                           node_string.size() - 2u };

            m_result_node = std::make_unique<string_value_node>(type, string_without_quotation_marks);
        }

        void sema_builder_ast_visitor::visit(const ast::id_node& node)
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

        void sema_builder_ast_visitor::visit(const ast::return_node& node)
        {
            auto v = clone();
            node.get_expression().visit(v);
            if(!v.m_result_node)
            {
                return;
            }

            auto expr = to_expression(std::move(v.m_result_node));
            expr = convert_to_cast_return_node_if_need(std::move(expr));
            m_result_node = std::make_unique<return_node>(std::move(expr));
        }

        void sema_builder_ast_visitor::visit(const ast::translation_unit_node& node)
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

        void sema_builder_ast_visitor::visit(const ast::user_function_node2& node)
        {
            const auto return_type_reference = node.get_return_type_reference();
            auto return_type = try_get_or_create_generic_type(m_ctx, return_type_reference);
            if(!return_type)
            {
                return;
            }

            // Todo: parameters need to be in the same ids context as function body.
            auto params_ids_guard = ids_guard();

            using param_decl_t = parameter_declaration;
            std::vector<param_decl_t> params;

            for(const auto& param_decl : node.get_param_declarations())
            {
                auto param_type = try_get_or_create_generic_type(m_ctx, param_decl.ty);
                if(!param_type)
                {
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

            // Store pointer to function that is currently parsed,
            // so function body will be able to figure out function return type
            // and make casted return expression nodes as needed.
            m_currently_parsed_function = &function;
            auto block = visit_child_node<block_node>(node.get_body());
            m_currently_parsed_function = nullptr;
            if(!block)
            {
                return;
            }

            // And set the body.
            function.set_body(*block);

            m_result_node = std::make_unique<function_node>(function, std::move(block));
        }

        void sema_builder_ast_visitor::visit(const ast::variable_declaration_node& node)
        {
            const auto& type_representation = node.get_type_representation();
            const auto type = try_get_or_create_generic_type(m_ctx, type_representation);
            if(!type)
            {
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

                variable_initialization_checker checker;
                const auto check_result = checker.check(*type, *initialization);

                switch(check_result)
                {
                    case variable_initialization_checker::check_result::can_init:
                    {
                        // Can initialize, do nothing.
                    }break;
                    case variable_initialization_checker::check_result::different_types:
                    {
                        // Todo: Init does not have same type as declared.
                        // Todo: introduce auto
                        raise_error(initialization->type().name().primary_name(), "Initialization and declared variable type does not match");
                        return;
                    }
                    case variable_initialization_checker::check_result::reference_init_from_temporary_value:
                    {
                        raise_error(initialization->type().name().primary_name(), "Reference variable can not be initialized with a temporary value");
                        return;
                    }
                }

                initialization = convert_to_cast_node_if_need(*type, std::move(initialization));
            }
            else
            {
                if(type->is_reference())
                {
                    raise_error(initialization->type().name().primary_name(), "Declaration of a reference variable requires an initializer");
                    return;
                }
            }

            m_ids_context.register_identifier(node.get_name(), type);
            m_result_node = std::make_unique<variable_declaration_node>(*type, node.get_name(), std::move(initialization));
        }

        void sema_builder_ast_visitor::visit(const ast::while_node& node)
        {
            auto conditional = visit_child_node<conditional_node>(node.get_node());
            if(!conditional)
            {
                return;
            }

            m_result_node = std::make_unique<while_node>(std::move(conditional));
        }

        const sema_type* sema_builder_ast_visitor::try_get_or_create_generic_type(const sema_context_interface& search_context, const ast::type_representation& name)
        {
            const auto found = search_context.find_type(name);
            if(found)
            {
                return found;
            }

            if(!name.is_generic())
            {
                raise_error(name.primary_name(), name.to_string() + " type not found.");
                return nullptr;
            }

            auto factory = sema_generic_type_factory{ m_generic_types_context,
                                                      search_context,
                                                      m_type_factory,
                                                      m_function_factory,
                                                      m_context_factory };

            // Todo: Add assert.
            return factory.create_generic(name);
        }

        template <typename T>
        std::unique_ptr<T> sema_builder_ast_visitor::to_node(std::unique_ptr<sema_node> node) const
        {
            return std::unique_ptr<T>(dynamic_cast<T*>(node.release()));
        }

        std::unique_ptr<expression_node> sema_builder_ast_visitor::to_expression(std::unique_ptr<sema_node> node) const
        {
            return std::unique_ptr<expression_node>(dynamic_cast<expression_node*>(node.release()));
        }

        void sema_builder_ast_visitor::raise_error(const lexer::token::token token, const std::string& message)
        {
            const auto err = errors::error{
                token.source().path(),
                token.source().line(token.src_range().begin.line),
                message,
                errors::error_type::error,
                token.src_range()
            };
            m_errors_observer.nofify_error(err); // todo: get error via parameter
        }

        sema_builder_ast_visitor sema_builder_ast_visitor::clone() const
        {
            return clone(m_ctx);
        }

        sema_builder_ast_visitor sema_builder_ast_visitor::clone(sema_context_interface& ctx_to_visit) const
        {
            return sema_builder_ast_visitor{ m_generic_types_context,
                                             ctx_to_visit,
                                             m_errors_observer,
                                             m_ids_context,
                                             m_type_factory,
                                             m_function_factory,
                                             m_context_factory,
                                             m_add_subdirectory_handler,
                                             m_currently_parsed_function };
        }

        std::unique_ptr<expression_node> sema_builder_ast_visitor::visit_child_expr(const ast::ast_node& node)
        {
            return to_node<expression_node>(visit_child(node));
        }

        template <typename T>
        std::unique_ptr<T> sema_builder_ast_visitor::visit_child_node(const ast::ast_node& node)
        {
            return to_node<T>(visit_child(node));
        }

        template <typename T>
        std::unique_ptr<T> sema_builder_ast_visitor::visit_child_node(const ast::ast_node& node, sema_context_interface& ctx_to_visit)
        {
            return to_node<T>(visit_child(node, ctx_to_visit));
        }

        std::unique_ptr<sema_node> sema_builder_ast_visitor::visit_child(const ast::ast_node& node)
        {
            auto v = clone();
            node.visit(v);
            return std::move(v.m_result_node);
        }

        std::unique_ptr<sema_node> sema_builder_ast_visitor::visit_child(const ast::ast_node& node, sema_context_interface& ctx_to_visit)
        {
            auto v = clone(ctx_to_visit);
            node.visit(v);
            return std::move(v.m_result_node);
        }

        boost::optional<sema_builder_ast_visitor::param_expressions_t> sema_builder_ast_visitor::get_function_call_params(const ast::call_node::params_t& passed_params)
        {
//                const auto& param_declarations = signature.params;

            /*
            if(param_declarations.size() != passed_params.size())
            {
                // Todo passed wrong number of parameters
                raise_error(signature.name, "Expected " + std::to_string(param_declarations.size()) + " parameters, got " + std::to_string(passed_params.size()));
                return {};
            }
             */

            std::vector<std::unique_ptr<expression_node>> params;

            for(auto i = 0u; i < passed_params.size(); ++i)
            {
                auto param = visit_child_expr(*passed_params[i]);
                if(!param)
                {
                    return {};
                }

                /*
                if(param_declarations[i].ty != param->type())
                {
                    //Todo passed param type mismatch
                    // Todo: pass param tokens range instead of empty token
                    raise_error(lexer::token::token{}, "Passed parameter does not match, expected " + param_declarations[i].ty.name().str().to_string()
                    + " got " + param->type().name().str().to_string());
                    return {};
                }
                 */

                params.emplace_back(std::move(param));
            }

            return params;
        }



sema_builder_ast_visitor::ids_ctx_guard sema_builder_ast_visitor::ids_guard()
        {
            return ids_ctx_guard{ m_ids_context };
        }

        boost::optional<sema_builder_ast_visitor::function_declaration> sema_builder_ast_visitor::get_function_declaration_and_add_to_ctx(const ast::user_function_node2& node,
                                                                                      sema_context& ctx)
        {
            const auto return_type_reference = node.get_return_type_reference();
            auto return_type = try_get_or_create_generic_type(ctx, return_type_reference);
            if(!return_type)
            {
                return {};
            }

            // Todo: parameters need to be in the same ids context as function body.
            auto params_ids_guard = ids_guard();

            std::vector<parameter_declaration> params;

            for(const auto& param_decl : node.get_param_declarations())
            {
                auto param_type = try_get_or_create_generic_type(ctx, param_decl.ty);
                if(!param_type)
                {
                    //Todo: unknown parameter type
                    raise_error(param_decl.ty.primary_name(), param_decl.ty.to_string() + " unknown parameter type");
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

        boost::optional<sema_builder_ast_visitor::class_members> sema_builder_ast_visitor::collect_class_members_and_add_functions_to_ctx(const ast::class_node2& node,
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

        template<unsigned N>
        lexer::token::token sema_builder_ast_visitor::make_token(lexer::token::token_type token_type, const char (&tok)[N])
        {
            // N counts also '\0'
            const auto src_range = source_range{
                    source_location{ 1u, 1u, 0u },
                    source_location{ 1u, N, N - 1u }
            };
            return lexer::token::token{ token_type, src_range, source_view{ tok } };
        }

        std::unique_ptr<expression_node>
        sema_builder_ast_visitor::convert_to_cast_return_node_if_need(std::unique_ptr<expression_node> expression)
        {
            if(!m_currently_parsed_function)
            {
                return std::move(expression);
            }

            const auto& expected_result_type = m_currently_parsed_function->return_type();
            return convert_to_cast_node_if_need(expected_result_type, std::move(expression));
        }


        std::unique_ptr<expression_node>
        sema_builder_ast_visitor::convert_to_cast_node_if_need(const sema_type& expected_result_type,
                                                               std::unique_ptr<expression_node> expression)
        {
            if(expected_result_type == expression->type())
            {
                return std::move(expression);
            }

            if(expected_result_type.is_reference())
            {
                return std::make_unique<cast_to_reference_node>(expected_result_type, std::move(expression));
            }
            else
            {
                return std::make_unique<cast_to_value_node>(expected_result_type, std::move(expression));
            }
        }

        sema_builder_ast_visitor::param_expressions_t
        sema_builder_ast_visitor::convert_params_to_cast_nodes_if_need(const function_signature &signature, param_expressions_t params)
        {
            for(auto i = 0u; i < params.size(); ++i)
            {
                const auto& declared_param_type = signature.params[i].ty;
                params[i] = convert_to_cast_node_if_need(declared_param_type, std::move(params[i]));
            }

            return std::move(params);
        }

        void sema_builder_ast_visitor::visit(const ast::initializer_list_node &node)
        {
            if(node.values().empty())
            {
                raise_error(node.begin_end().first, "initializer lsit must contain values");
                return;
            }

            std::vector<std::unique_ptr<expression_node>> expression_values;
            const sema_type* value_type{ nullptr };

            for(const auto& value : node.values())
            {
                auto expression = visit_child_expr(*value);
                if(!expression)
                {
                    return;
                }

                const auto& expr_type = expression->type();
                if(value_type && *value_type != expr_type)
                {
                    raise_error(node.begin_end().first, "all values of initializer list must have the same type");
                    return;
                }

                if(!value_type)
                {
                    value_type = &expr_type;
                }

                expression_values.emplace_back(std::move(expression));
            }

            // Todo: Introduce utilityt to create generic name representations
            using token_type_t = lexer::token::token_type;
            lexer::token::token_container_t tokens{
                make_token(token_type_t::kw_list, "list"),
                make_token(token_type_t::less, "<")
            };
            const auto& value_type_tokens = value_type->name().tokens();
            tokens.insert(std::end(tokens), std::cbegin(value_type_tokens), std::cend(value_type_tokens));
            tokens.emplace_back(make_token(token_type_t::greater, ">"));

            const auto list_type_representation = ast::type_representation{
                std::move(tokens),
                { value_type->name() }
            };

            auto type = try_get_or_create_generic_type(m_ctx, list_type_representation);
            if(!type)
            {
                // Should be impossible to get here.
                raise_error(node.begin_end().first, "could not create initalizer_list type");
                return;
            }

            m_result_node = std::make_unique<initializer_list_node>(*type, std::move(expression_values));
        }
    }
}
