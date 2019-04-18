#pragma once

#include "ast/ast_node.hpp"

#include "lexer/token/token.hpp"

#include "sema/sema_node.hpp"
#include "sema/sema_node_visitor.hpp"
#include "sema/sema_function.hpp"
#include "sema/sema_type.hpp"

#include <memory>

#define VISIT_METHOD \
void visit(sema_node_visitor& visitor) const override \
{ \
    visitor.visit(*this); \
}

namespace cmsl::sema
{
        class expression_node : public sema_node
        {
        public:
            using sema_node::sema_node;

            virtual const sema_type& type() const = 0;
            virtual bool produces_temporary_value() const = 0;
        };

        template <typename T>
        class value_node : public expression_node
        {
        public:
            explicit value_node(const ast::ast_node& ast_node, const sema_type& t, T val)
                : expression_node{ ast_node }
                , m_type{ t }
                , m_value{ val }
            {}

            T value() const
            {
                return m_value;
            }

            const sema_type& type() const override
            {
                return m_type;
            }

            bool produces_temporary_value() const override
            {
                return true;
            }

        private:
            const sema_type& m_type;
            T m_value;
        };

        class bool_value_node : public value_node<bool>
        {
        public:
            explicit bool_value_node(const ast::ast_node& ast_node, const sema_type& t, bool val)
                : value_node{ ast_node, t, val }
            {}

            VISIT_METHOD
        };

        // Todo: move int alias to a common file
        class int_value_node : public value_node<std::int64_t>
        {
        public:
            explicit int_value_node(const ast::ast_node& ast_node, const sema_type& t, std::int64_t val)
                    : value_node{ ast_node, t, val }
            {}

            VISIT_METHOD
        };

        class double_value_node : public value_node<double>
        {
        public:
            explicit double_value_node(const ast::ast_node& ast_node, const sema_type& t, double val)
                    : value_node{ast_node,  t, val }
            {}

            VISIT_METHOD
        };

        class string_value_node : public value_node<cmsl::string_view>
        {
        public:
            explicit string_value_node(const ast::ast_node& ast_node, const sema_type& t, cmsl::string_view val)
                    : value_node{ ast_node, t, val }
            {}

            VISIT_METHOD
        };

        class id_node : public expression_node
        {
        public:
            explicit id_node(const ast::ast_node& ast_node, const sema_type& t, lexer::token::token id)
                : expression_node{ ast_node }
                , m_type{ t }
                , m_id{ id }
            {}

            const sema_type& type() const override
            {
                return m_type;
            }

            bool produces_temporary_value() const override
            {
                return false;
            }

            // Todo: consider renaming getters to get_*
            lexer::token::token id() const
            {
                return m_id;
            }

            VISIT_METHOD

        private:
            const sema_type& m_type;
            lexer::token::token m_id;
        };

        // Todo: handle return without expression
        class return_node : public expression_node
        {
        public:
            explicit return_node(const ast::ast_node& ast_node, std::unique_ptr<expression_node> expr)
                : expression_node{ ast_node }
                , m_expr{ std::move(expr) }
            {
                m_expr->set_parent(*this, passkey{});
            }

            const sema_type& type() const override
            {
                return m_expr->type();
            }

            bool produces_temporary_value() const override
            {
                return !type().is_reference();
            }

            const expression_node& expression() const
            {
                return *m_expr;
            }

            VISIT_METHOD

        private:
            std::unique_ptr<expression_node> m_expr;
        };

        class binary_operator_node : public expression_node
        {
        public:
            explicit binary_operator_node(const ast::ast_node& ast_node,
                                          std::unique_ptr<expression_node> lhs,
                                          lexer::token::token op,
                                          const sema_function& operator_function,
                                          std::unique_ptr<expression_node> rhs,
                                          const sema_type& result_type)
                : expression_node{ ast_node }
                , m_lhs{ std::move(lhs) }
                , m_operator{ op }
                , m_operator_function{ operator_function }
                , m_rhs{ std::move(rhs) }
                , m_type{ result_type }
            {
                m_lhs->set_parent(*this, passkey{});
                m_rhs->set_parent(*this, passkey{});
            }

            const expression_node& lhs() const
            {
                return *m_lhs;
            }

            lexer::token::token op() const
            {
                return m_operator;
            }

            const sema_function& operator_function() const
            {
                return m_operator_function;
            }

            const expression_node& rhs() const
            {
                return *m_rhs;
            }

            const sema_type& type() const override
            {
                return m_type;
            }

            bool produces_temporary_value() const override
            {
                return !type().is_reference();
            }

            VISIT_METHOD

        private:
            std::unique_ptr<expression_node> m_lhs;
            lexer::token::token m_operator; // Todo: introduce an operator struct that holds token and operator type
            const sema_function& m_operator_function;
            std::unique_ptr<expression_node> m_rhs;
            const sema_type& m_type;
        };

        class variable_declaration_node : public sema_node
        {
        public:
            explicit variable_declaration_node(const ast::ast_node& ast_node, const sema_type& type, lexer::token::token name, std::unique_ptr<expression_node> initialization)
                    : sema_node{ ast_node }
                    , m_type{ type }
                    , m_name{ name }
                    , m_initialization{ std::move(initialization) }
            {
                if(initialization)
                {
                    initialization->set_parent(*this, passkey{});
                }
            }

            const sema_type& type() const
            {
                return m_type;
            }

            lexer::token::token name() const
            {
                return m_name;
            }

            const sema_node* initialization() const
            {
                return m_initialization.get();
            }

            VISIT_METHOD

        private:
            const sema_type& m_type;
            const lexer::token::token m_name;
            std::unique_ptr<expression_node> m_initialization;
        };

        class call_node : public expression_node
        {
        public:
            using param_expressions_t = std::vector<std::unique_ptr<expression_node>>;

            explicit call_node(const ast::ast_node& ast_node, const sema_function& function, param_expressions_t params)
                    : expression_node{ ast_node }
                    , m_function{ function }
                    , m_params{ std::move(params) }
            {
                for(auto& param : m_params)
                {
                    param->set_parent(*this, passkey{});
                }
            }

            const sema_type& type() const override
            {
                return m_function.return_type();
            }

            const param_expressions_t& param_expressions() const
            {
                return m_params;
            }

            lexer::token::token name() const
            {
                return m_function.signature().name;
            }

            const sema_function& function() const
            {
                return m_function;
            }

            bool produces_temporary_value() const override
            {
                return !type().is_reference();
            }

        private:
            const sema_function& m_function;
            param_expressions_t m_params;
        };

        class function_call_node : public call_node
        {
        public:
            explicit function_call_node(const ast::ast_node& ast_node, const sema_function& function, param_expressions_t params)
                : call_node{ast_node, function, std::move(params) }
            {}

            VISIT_METHOD
        };

        class member_function_call_node : public call_node
        {
        public:
            explicit member_function_call_node(const ast::ast_node& ast_node, std::unique_ptr<expression_node> lhs, const sema_function& function, param_expressions_t params)
                : call_node{ast_node, function, std::move(params) }
                , m_lhs{ std::move(lhs) }
            {
                m_lhs->set_parent(*this, passkey{});
            }

            const expression_node& lhs() const
            {
                return *m_lhs;
            }

            VISIT_METHOD

        private:
            std::unique_ptr<expression_node> m_lhs;
        };

        class implicit_member_function_call_node : public call_node
        {
        public:
            explicit implicit_member_function_call_node(const ast::ast_node& ast_node, const sema_function& function, param_expressions_t params)
                    : call_node{ast_node, function, std::move(params) }
            {}

            VISIT_METHOD
        };

        class constructor_call_node : public call_node
        {
        public:
            explicit constructor_call_node(const ast::ast_node& ast_node, const sema_type& class_type, const sema_function& function, param_expressions_t params)
                : call_node{ast_node, function, std::move(params) }
                , m_class_type{ class_type }
            {}

            const sema_type& type() const override
            {
                return m_class_type;
            }

            VISIT_METHOD

        private:
            const sema_type& m_class_type;
        };

        class add_subdirectory_node : public call_node
        {
        public:
            explicit add_subdirectory_node(const ast::ast_node& ast_node, std::unique_ptr<string_value_node> directory_name, const sema_function& function, param_expressions_t params)
                    : call_node{ast_node, function, std::move(params) }
                    , m_directory_name{ std::move(directory_name) }
            {
                m_directory_name->set_parent(*this, passkey{});
            }

            const string_value_node& dir_name() const
            {
                return *m_directory_name;
            }

            VISIT_METHOD

        private:
            std::unique_ptr<string_value_node> m_directory_name;
        };

        class block_node : public sema_node
        {
        private:
            using nodes_t = std::vector<std::unique_ptr<sema_node>>;

        public:
            explicit block_node(const ast::ast_node& ast_node, nodes_t nodes)
                : sema_node{ ast_node }
                , m_nodes{ std::move(nodes) }
            {
                for(auto& node : m_nodes)
                {
                    node -> set_parent(*this, passkey{});
                }
            }

            const nodes_t& nodes() const
            {
                return m_nodes;
            }

            VISIT_METHOD

        private:
            nodes_t m_nodes;
        };

        class function_node : public sema_node
        {
        public:
            explicit function_node(const ast::ast_node& ast_node, const sema_function& function, std::unique_ptr<block_node> body)
                : sema_node{ ast_node }
                , m_function{ function }
                , m_body{ std::move(body) }
            {
                m_body->set_parent(*this, passkey{});
            }

            const function_signature& signature() const
            {
                return m_function.signature();
            }

            const sema_function& function() const
            {
                return m_function;
            }

            const sema_type& return_type() const
            {
                return m_function.return_type();
            }

            const block_node& body() const
            {
                return *m_body;
            }

            VISIT_METHOD

        private:
            const sema_function& m_function;
            std::unique_ptr<block_node> m_body;
        };

        class class_node : public sema_node
        {
        private:
            using members_t = std::vector<std::unique_ptr<variable_declaration_node>>;
            using functions_t = std::vector<std::unique_ptr<function_node>>;

        public:
            explicit class_node(const ast::ast_node& ast_node, lexer::token::token name, members_t members, functions_t functions)
                : sema_node{ ast_node }
                , m_name{ name }
                , m_members{ std::move(members) }
                , m_functions{ std::move(functions) }
            {
                for(auto& member : m_members)
                {
                    member->set_parent(*this, passkey{});
                }
                for(auto& function : m_functions)
                {
                    function->set_parent(*this, passkey{});
                }
            }

            lexer::token::token name() const
            {
                return m_name;
            }

            const members_t& members() const
            {
                return m_members;
            }

            const functions_t& functions() const
            {
                return m_functions;
            }

            VISIT_METHOD

        private:
            lexer::token::token m_name;
            members_t m_members;
            functions_t m_functions;
        };

        class conditional_node : public sema_node
        {
        public:
            explicit conditional_node(const ast::ast_node& ast_node, std::unique_ptr<expression_node> condition, std::unique_ptr<block_node> body)
                : sema_node{ ast_node }
                , m_condition{ std::move(condition)}
                , m_body{ std::move(body)}
            {
                m_condition->set_parent(*this, passkey{});
                m_body->set_parent(*this, passkey{});
            }

            const expression_node& get_condition() const
            {
                return *m_condition;
            }

            const block_node& get_body() const
            {
                return *m_body;
            }

            VISIT_METHOD

        private:
            std::unique_ptr<expression_node> m_condition;
            std::unique_ptr<block_node> m_body;
        };

        class while_node : public sema_node
        {
        public:
            explicit while_node(const ast::ast_node& ast_node, std::unique_ptr<conditional_node> condition)
                : sema_node{ ast_node }
                , m_conditional{ std::move(condition)}
            {
                m_conditional->set_parent(*this, passkey{});
            }

            const conditional_node& condition() const
            {
                return *m_conditional;
            }

            VISIT_METHOD

        private:
            std::unique_ptr<conditional_node> m_conditional;
        };

        class if_else_node : public sema_node
        {
        public:
            using ifs_t = std::vector<std::unique_ptr<conditional_node>>;

            explicit if_else_node(const ast::ast_node& ast_node, ifs_t ifs, std::unique_ptr<block_node> else_node)
                : sema_node{ ast_node }
                , m_ifs{ std::move(ifs) }
                , m_else{ std::move(else_node) }
            {
                for(auto& if_ : m_ifs)
                {
                    if_->set_parent(*this, passkey{});
                }

                if(m_else)
                {
                    m_else->set_parent(*this, passkey{});
                }
            }

            const ifs_t& ifs() const
            {
                return m_ifs;
            }

            const block_node* else_body() const
            {
                return m_else.get();
            }

            VISIT_METHOD

        private:
            ifs_t m_ifs;
            std::unique_ptr<block_node> m_else;
        };

        class class_member_access_node : public expression_node
        {
        public:
            explicit class_member_access_node(const ast::ast_node& ast_node, std::unique_ptr<expression_node> lhs, member_info member_info)
                : expression_node{ ast_node }
                , m_lhs{ std::move(lhs) }
                , m_member_info{ member_info }
            {
                m_lhs->set_parent(*this, passkey{});
            }

            const expression_node& lhs() const
            {
                return *m_lhs;
            }

            const sema_type& type() const override
            {
                return m_member_info.ty;
            }

            bool produces_temporary_value() const override
            {
                return false;
            }

            lexer::token::token member_name() const
            {
                return m_member_info.name;
            }

            VISIT_METHOD

        private:
            std::unique_ptr<expression_node> m_lhs;
            member_info m_member_info;
        };

        class translation_unit_node : public sema_node
        {
        public:
            using nodes_t = std::vector<std::unique_ptr<sema_node>>;

            explicit translation_unit_node(const ast::ast_node& ast_node, const sema_context_interface& ctx, nodes_t nodes)
                : sema_node{ ast_node }
                , m_ctx{ ctx }
                , m_nodes{std::move(nodes)}
            {
                for(auto& node : m_nodes)
                {
                    node->set_parent(*this, passkey{});
                }
            }

            const nodes_t& nodes() const
            {
                return m_nodes;
            }

            const sema_context_interface& context() const
            {
                return m_ctx;
            }

            VISIT_METHOD

        private:
            const sema_context_interface& m_ctx;
            nodes_t m_nodes;
        };

        class cast_to_reference_node : public expression_node
        {
        public:
            explicit cast_to_reference_node(const ast::ast_node& ast_node, const sema_type& t, std::unique_ptr<expression_node> expr)
                    : expression_node{ ast_node }
                    , m_type{ t }
                    , m_expr{ std::move(expr) }
            {
                m_expr->set_parent(*this, passkey{});
            }

            const sema_type& type() const override
            {
                return m_type;
            }

            bool produces_temporary_value() const override
            {
                return false;
            }

            const expression_node& expression() const
            {
                return *m_expr;
            }

            VISIT_METHOD

        private:
            const sema_type& m_type;
            std::unique_ptr<expression_node> m_expr;
        };

        class cast_to_value_node : public expression_node
        {
        public:
            explicit cast_to_value_node(const ast::ast_node& ast_node, const sema_type& t, std::unique_ptr<expression_node> expr)
                    : expression_node{ ast_node }
                    , m_type{ t }
                    , m_expr{ std::move(expr) }
            {
                m_expr->set_parent(*this, passkey{});
            }

            const sema_type& type() const override
            {
                return m_type;
            }

            bool produces_temporary_value() const override
            {
                return false;
            }

            const expression_node& expression() const
            {
                return *m_expr;
            }

            VISIT_METHOD

        private:
            const sema_type& m_type;
            std::unique_ptr<expression_node> m_expr;
        };

        class initializer_list_node : public expression_node
        {
        public:
            explicit initializer_list_node(const ast::ast_node& ast_node, const sema_type& t, std::vector<std::unique_ptr<expression_node>> values)
                : expression_node{ ast_node }
                , m_type{ t }
                , m_values{ std::move(values) }
            {
                for(auto& value : m_values)
                {
                    value->set_parent(*this, passkey{});
                }
            }

            const sema_type& type() const override
            {
                return m_type;
            }

            bool produces_temporary_value() const override
            {
                return true;
            }

            const std::vector<std::unique_ptr<expression_node>>& values() const
            {
                return m_values;
            }

            VISIT_METHOD

        private:
            const sema_type& m_type;
            std::vector<std::unique_ptr<expression_node>> m_values;
        };
}
