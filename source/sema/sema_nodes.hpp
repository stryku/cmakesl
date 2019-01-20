#pragma once

#include "lexer/token/token.hpp"

#include "sema/sema_node.hpp"
#include "sema/sema_node_visitor.hpp"

namespace cmsl
{
    namespace ast
    {
        class type;
    }

    namespace sema
    {
        class expression_node : public sema_node
        {
        public:
            virtual const ast::type& type() const = 0;
        };

        template <typename T>
        class value_node : public expression_node
        {
        public:
            explicit value_node(const ast::type& t, T val)
                : m_type{ t }
                , m_value{ val }
            {}

            T value() const
            {
                return m_value;
            }

            const ast::type& type() const override
            {
                return m_type;
            }

        private:
            const ast::type& m_type;
            T m_value;
        };

        class bool_value_node : public value_node<bool>
        {
        public:
            explicit bool_value_node(const ast::type& t, bool val)
                : value_node{ t, val }
            {}

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }
        };

        // Todo: move int alias to a common file
        class int_value_node : public value_node<std::int64_t>
        {
        public:
            explicit int_value_node(const ast::type& t, std::int64_t val)
                    : value_node{ t, val }
            {}

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }
        };

        class double_value_node : public value_node<double>
        {
        public:
            explicit double_value_node(const ast::type& t, double val)
                    : value_node{ t, val }
            {}

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }
        };

        class string_value_node : public value_node<cmsl::string_view>
        {
        public:
            explicit string_value_node(const ast::type& t, cmsl::string_view val)
                    : value_node{ t, val }
            {}

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }
        };

        class id_node : public expression_node
        {
        public:
            explicit id_node(const ast::type& t, lexer::token::token id)
                : m_type{ t }
                , m_id{ id }
            {}

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }

            const ast::type& type() const override
            {
                return m_type;
            }

            // Todo: consider renaming getters to get_*
            lexer::token::token id() const
            {
                return m_id;
            }

        private:
            const ast::type& m_type;
            lexer::token::token m_id;
        };

        // Todo: handle return without expression
        class return_node : public expression_node
        {
        public:
            explicit return_node(std::unique_ptr<expression_node> expr)
                : m_expr{ std::move(expr) }
            {}

            const ast::type& type() const override
            {
                return m_expr->type();
            }

            const expression_node& expression() const
            {
                return *m_expr;
            }

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }

        private:
            std::unique_ptr<expression_node> m_expr;
        };

        class binary_operator_node : public expression_node
        {
        public:
            explicit binary_operator_node(std::unique_ptr<expression_node> lhs, lexer::token::token op, std::unique_ptr<expression_node> rhs, const ast::type& result_type)
                : m_lhs{ std::move(lhs) }
                , m_operator{ op }
                , m_rhs{ std::move(rhs) }
                , m_type{ result_type }
            {}

            const expression_node& lhs() const
            {
                return *m_lhs;
            }

            lexer::token::token op() const
            {
                return m_operator;
            }

            const expression_node& rhs() const
            {
                return *m_rhs;
            }

            const ast::type& type() const override
            {
                return m_type;
            }

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }

        private:
            std::unique_ptr<expression_node> m_lhs;
            lexer::token::token m_operator; // Todo: introduce an operator struct that holds token and operator type
            std::unique_ptr<expression_node> m_rhs;
            const ast::type& m_type;
        };

        class variable_declaration_node : public sema_node
        {
        public:
            explicit variable_declaration_node(const ast::type& type, lexer::token::token name, std::unique_ptr<expression_node> initialization)
                    : m_type{ type }
                    , m_name{ name }
                    , m_initialization{ std::move(initialization) }
            {}

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }

            const ast::type& type() const
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

        private:
            const ast::type& m_type;
            const lexer::token::token m_name;
            std::unique_ptr<expression_node> m_initialization;
        };

        class call_node : public expression_node
        {
        public:
            using param_expressions_t = std::vector<std::unique_ptr<expression_node>>;

            explicit call_node(const ast::type& return_type, lexer::token::token name, param_expressions_t params)
                    : m_return_type{ return_type }
                    , m_name{ name }
                    , m_params{ std::move(params) }
            {}

            const ast::type& type() const override
            {
                return m_return_type;
            }

            const param_expressions_t& param_expressions() const
            {
                return m_params;
            }

        private:
            const ast::type& m_return_type;
            lexer::token::token m_name;
            param_expressions_t m_params;
        };

        class function_call_node : public call_node
        {
        public:
            explicit function_call_node(const ast::type& return_type, lexer::token::token name, param_expressions_t params)
                : call_node{ return_type, name, std::move(params) }
            {}

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }
        };

        class member_function_call_node : public call_node
        {
        public:
            explicit member_function_call_node(std::unique_ptr<expression_node> lhs, const ast::type& return_type, lexer::token::token name, param_expressions_t params)
                : call_node{ return_type, name, std::move(params) }
                , m_lhs{ std::move(lhs) }
            {}

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }

            const expression_node& lhs() const
            {
                return *m_lhs;
            }

        private:
            std::unique_ptr<expression_node> m_lhs;
        };

        class block_node : public sema_node
        {
        private:
            using nodes_t = std::vector<std::unique_ptr<sema_node>>;

        public:
            explicit block_node(nodes_t nodes)
                : m_nodes{ std::move(nodes) }
            {}

            const nodes_t& nodes() const
            {
                return m_nodes;
            }

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }

        private:
            nodes_t m_nodes;
        };

        class function_node : public sema_node
        {
        public:
            struct parameter_declaration
            {
                const ast::type& type;
                lexer::token::token name;
            };

            explicit function_node(const ast::type& return_type, std::vector<parameter_declaration> params, std::unique_ptr<block_node> body)
            : m_return_type{ return_type }
            ,m_params{ std::move(params) }
            , m_body{ std::move(body) }
            {}

            const std::vector<parameter_declaration>& params() const
            {
                return m_params;
            }

            const block_node& body() const
            {
                return *m_body;
            }

            const ast::type& return_type() const
            {
                return m_return_type;
            }

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }

        private:
            const ast::type& m_return_type;
            std::vector<parameter_declaration> m_params;
            std::unique_ptr<block_node> m_body;
        };

        class class_node : public sema_node
        {
        private:
            using members_t = std::vector<std::unique_ptr<variable_declaration_node>>;
            using functions_t = std::vector<std::unique_ptr<function_node>>;

        public:
            explicit class_node(lexer::token::token name, members_t members/*, functions_t functions*/)
                : m_name{ name }
                , m_members{ std::move(members) }
                //, m_functions{ std::move(functions) }
            {}

            lexer::token::token name() const
            {
                return m_name;
            }

            const members_t& members() const
            {
                return m_members;
            }

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }

        private:
            lexer::token::token m_name;
            members_t m_members;
            //functions_t m_functions;
        };

        class conditional_node : public sema_node
        {
        public:
            explicit conditional_node(std::unique_ptr<expression_node> condition, std::unique_ptr<block_node> body)
                : m_condition{ std::move(condition)}
                , m_body{ std::move(body)}
            {}

            const expression_node& get_condition() const
            {
                return *m_condition;
            }

            const block_node& get_body() const
            {
                return *m_body;
            }

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }

        private:
            std::unique_ptr<expression_node> m_condition;
            std::unique_ptr<block_node> m_body;
        };

        class while_node : public sema_node
        {
        public:
            explicit while_node(std::unique_ptr<conditional_node> condition)
                : m_conditional{ std::move(condition)}
            {}

            const conditional_node& condition() const
            {
                return *m_conditional;
            }

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }

        private:
            std::unique_ptr<conditional_node> m_conditional;
        };

        class if_else_node : public sema_node
        {
        public:
            using ifs_t = std::vector<std::unique_ptr<conditional_node>>;

            explicit if_else_node(ifs_t ifs, std::unique_ptr<block_node> else_node)
                : m_ifs{ std::move(ifs) }
                , m_else{ std::move(else_node) }
            {}

            const ifs_t& ifs() const
            {
                return m_ifs;
            }

            const block_node* else_body() const
            {
                return m_else.get();
            }

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }

        private:
            ifs_t m_ifs;
            std::unique_ptr<block_node> m_else;
        };

        class class_member_access_node : public expression_node
        {
        public:
            explicit class_member_access_node(std::unique_ptr<expression_node> lhs, ast::type::member_info member_info)
                : m_lhs{ std::move(lhs) }
                , m_member_info{ member_info }
            {}

            const expression_node& lhs() const
            {
                return *m_lhs;
            }

            const ast::type& type() const override
            {
                return m_member_info.ty;
            }

            lexer::token::token member_name() const
            {
                return m_member_info.name;
            }

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }

        private:
            std::unique_ptr<expression_node> m_lhs;
            ast::type::member_info m_member_info;
        };

        class translation_unit_node : public sema_node
        {
        public:
            using nodes_t = std::vector<std::unique_ptr<sema_node>>;

            explicit translation_unit_node(nodes_t nodes)
                : m_nodes{std::move(nodes)}
            {}

            const nodes_t& nodes() const
            {
                return m_nodes;
            }

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }

        private:
            nodes_t m_nodes;
        };
    }
}
