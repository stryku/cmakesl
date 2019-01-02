#pragma once

#include "ast/ast_node_visitor.hpp"
#include "ast/ast_context.hpp"

#include "ast/variable_declaration_node.hpp"
#include "ast/infix_nodes.hpp"

#include "common/algorithm.hpp"

#include "errors/error.hpp"
#include "errors/errors_observer.hpp"

#include "sema/sema_nodes.hpp"

namespace cmsl
{
    namespace sema
    {
        // Todo: consider moving to its own file
        class identifiers_context
        {
        private:
            using token_t = lexer::token::token;
            using id_map_t = std::unordered_map<token_t, const ast::type*>;

        public:
            void add(token_t name, const ast::type* ty)
            {
                // Todo: handle empty
                auto& ctx = m_contextes.back();
                ctx[name] = ty;
            }

            bool exists(cmsl::string_view name) const
            {
                return type_of(name) != nullptr;
            }

            const ast::type* type_of(cmsl::string_view name) const
            {
                const auto pred = [name](const auto& id_pair)
                {
                    return id_pair.first.str() == name;
                };

                // Iterate contextes 'from top to down'.
                for(auto ctx_it = std::crbegin(m_contextes); ctx_it != std::crend(m_contextes); ++ctx_it)
                {
                    const auto& ids = *ctx_it;
                    const auto found = std::find_if(std::cbegin(ids), std::cend(ids), pred);

                    if(found != std::cend(ids))
                    {
                        return found->second;
                    }
                }

                return nullptr;
            }

            // Returns guard that automatically leaves context at the end of its lifetime.
            auto new_ctx()
            {
                struct guard
                {
                    ~guard()
                    {
                        contextes_ref.pop_back();
                    }

                    std::vector<id_map_t>& contextes_ref;
                };

                m_contextes.emplace_back(id_map_t{});

                return guard{ m_contextes };
            }

            void leave_ctx()
            {
                m_contextes.pop_back();
            }

        private:
            std::vector<id_map_t> m_contextes;
        };

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
            void visit(const ast::binary_operator_node& node) override {}
            void visit(const ast::class_member_access_node& node) override {}
            void visit(const ast::function_call_node& node) override {}
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

            void visit(const ast::return_node& node) override {}
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

            std::unique_ptr<sema_node> m_result_node;

        private:
            void raise_error()
            {
                m_errors_observer.nofify_error({}); // todo: get error via parameter
            }

            sema_builder_ast_visitor clone() const
            {
                return sema_builder_ast_visitor{ m_ctx, m_errors_observer, m_ids_context };
            }

        private:
            ast::ast_context& m_ctx;
            errors::errors_observer& m_errors_observer;
            identifiers_context& m_ids_context;
        };
    }
}
