#pragma once

#include "ast/ast_node.hpp"
#include "ast/ast_node_visitor.hpp"
#include "lexer/token.hpp"

#include <memory>

namespace cmsl::ast
{
        class binary_operator_node : public ast_node
        {
        public:
            explicit binary_operator_node(std::unique_ptr<ast_node> lhs, token_t op,  std::unique_ptr<ast_node> rhs)
                : m_lhs{ std::move(lhs)}
                , m_operator{ op }
                , m_rhs{ std::move(rhs)}
            {}

            const ast_node& lhs() const
            {
                return *m_lhs;
            }

            lexer::token operator_() const
            {
                return m_operator;
            }

            const ast_node& rhs() const
            {
                return *m_rhs;
            }

            void visit(ast_node_visitor &visitor) const override
            {
                visitor.visit(*this);
            }

            source_location begin_location() const override
            {
                return m_lhs->begin_location();
            }

            source_location end_location() const override
            {
                return m_rhs->end_location();
            }

        private:
            std::unique_ptr<ast_node> m_lhs;
            token_t m_operator;
            std::unique_ptr<ast_node> m_rhs;
        };

        class class_member_access_node : public ast_node
        {
        public:
            explicit class_member_access_node(std::unique_ptr<ast_node> lhs, token_t dot, token_t member_name)
                : m_lhs{ std::move(lhs) }
                , m_dot{ dot }
                , m_member_name{ member_name }
            {}

            const ast_node& lhs() const
            {
                return *m_lhs;
            }

            lexer::token member_name() const
            {
                return m_member_name;
            }

            token_t  dot() const
            {
                return m_dot;
            }

            void visit(ast_node_visitor &visitor) const override
            {
                visitor.visit(*this);
            }

            source_location begin_location() const override
            {
                return m_lhs->begin_location();
            }

            source_location end_location() const override
            {
                return m_member_name.src_range().end;
            }

        private:
            std::unique_ptr<ast_node> m_lhs;
            token_t m_dot;
            token_t m_member_name;
        };

        class call_node : public ast_node
        {
        public:
            using params_t = std::vector<std::unique_ptr<ast_node>>;

            explicit call_node(token_t name, token_t open_paren, params_t parameter_nodes, token_t close_paren)
                : m_name{ std::move(name) }
                , m_open_paren{ open_paren }
                , m_parameter_nodes{ std::move(parameter_nodes) }
                , m_close_paren{ close_paren }
            {}

            token_t name() const
            {
                return m_name;
            }

            token_t open_paren() const
            {
                return m_open_paren;
            }

            token_t close_paren() const
            {
                return m_close_paren;
            }

            const params_t& param_nodes() const
            {
                return m_parameter_nodes;
            }

        private:
            token_t m_name;
            token_t m_open_paren;
            params_t m_parameter_nodes;
            token_t m_close_paren;
        };

        class function_call_node : public call_node
        {
        public:
            explicit function_call_node(token_t name,
                                        token_t open_paren,
                                        params_t params,
                                        token_t close_paren)
                : call_node{ name, open_paren,  std::move(params), close_paren }
            {}

            void visit(ast_node_visitor &visitor) const override
            {
                visitor.visit(*this);
            }

            source_location begin_location() const override
            {
                return name().src_range().begin;
            }

            source_location end_location() const override
            {
                return close_paren().src_range().end;
            }
        };

        class member_function_call_node : public call_node
        {
        public:
            explicit member_function_call_node(std::unique_ptr<ast_node> lhs,
                                               token_t dot,
                                               token_t name,
                                               token_t open_paren,
                                               params_t params,
                                               token_t close_paren)
                : call_node{ name, open_paren, std::move(params), close_paren }
                , m_lhs{ std::move(lhs) }
                , m_dot{ dot }
            {}

            const ast_node& lhs() const
            {
                return *m_lhs;
            }

            token_t dot() const
            {
                return m_dot;
            }

            void visit(ast_node_visitor &visitor) const override
            {
                visitor.visit(*this);
            }

            source_location begin_location() const override
            {
                return m_lhs->begin_location();
            }

            source_location end_location() const override
            {
                return close_paren().src_range().end;
            }

        private:
            std::unique_ptr<ast_node> m_lhs;
            token_t m_dot;
        };

        class fundamental_value_node : public ast_node
        {
        public:
            explicit fundamental_value_node(token_t token)
                : m_token{ token }
            {}

            token_t token() const
            {
                return m_token;
            }

            source_location begin_location() const override
            {
                return m_token.src_range().begin;
            }

            source_location end_location() const override
            {
                return m_token.src_range().end;
            }

        protected:
            const token_t m_token;
        };

        class bool_value_node : public fundamental_value_node
        {
        public:
            explicit bool_value_node(token_t token)
                : fundamental_value_node{token}
            {}

            void visit(ast_node_visitor &visitor) const override
            {
                visitor.visit(*this);
            }
        };

        class int_value_node : public fundamental_value_node
        {
        public:
            explicit int_value_node(token_t token)
                    : fundamental_value_node{token}
            {}

            void visit(ast_node_visitor &visitor) const override
            {
                visitor.visit(*this);
            }
        };

        class double_value_node : public fundamental_value_node
        {
        public:
            explicit double_value_node(token_t token)
                : fundamental_value_node{token}
            {}

            void visit(ast_node_visitor &visitor) const override
            {
                visitor.visit(*this);
            }
        };

        class string_value_node : public fundamental_value_node
        {
        public:
            explicit string_value_node(token_t token)
                : fundamental_value_node{token}
            {}

            void visit(ast_node_visitor &visitor) const override
            {
                visitor.visit(*this);
            }
        };

        class id_node : public ast_node
        {
        public:
            explicit id_node(token_t identifer)
                    : m_identifer{ identifer }
            {}

            token_t get_identifier() const
            {
                return m_identifer;
            }

            void visit(ast_node_visitor &visitor) const override
            {
                visitor.visit(*this);
            }

            source_location begin_location() const override
            {
                return m_identifer.src_range().begin;
            }

            source_location end_location() const override
            {
                return m_identifer.src_range().end;
            }

        private:
            lexer::token m_identifer;
        };

        class initializer_list_node : public ast_node
        {
        public:
            explicit initializer_list_node(token_t open_brace,
                                           std::vector<std::unique_ptr<ast_node>> values,
                                           token_t close_brace)
                    : m_open_brace{ open_brace }
                    , m_values{ std::move(values) }
                    , m_close_brace{ close_brace }
            {}

            const std::vector<std::unique_ptr<ast_node>>& values() const
            {
                return m_values;
            }

            token_t open_brace() const
            {
                return m_open_brace;
            }

            token_t close_brace() const
            {
                return m_close_brace;
            }

            void visit(ast_node_visitor &visitor) const override
            {
                visitor.visit(*this);
            }

            source_location begin_location() const override
            {
                return m_open_brace.src_range().begin;
            }

            source_location end_location() const override
            {
                return m_close_brace.src_range().end;
            }

        private:
            token_t m_open_brace;
            std::vector<std::unique_ptr<ast_node>> m_values;
            token_t m_close_brace;
        };
}
