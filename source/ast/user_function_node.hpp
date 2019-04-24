#pragma once

#include "ast/block_node.hpp"
#include "ast/type_representation.hpp"
#include "ast/ast_node_visitor.hpp" // todo: to cpp
#include "ast/parameter_declaration.hpp"

#include "lexer/token/token.hpp"

#include <memory>

namespace cmsl::ast
{
        class user_function_node : public ast_node
        {
        public:
            using params_t = std::vector<param_declaration>;

            user_function_node(type_representation return_type,
                                token_t name,
                                token_t open_paren,
                                params_t params,
                                token_t close_paren,
                                std::unique_ptr<block_node> body)
                    : m_return_type{ std::move(return_type) }
                    , m_name{ name }
                    , m_params{ std::move(params) }
                    , m_body{ std::move(body) }
            {}

            //todo: remove get_

            type_representation get_return_type_reference() const
            {
                return m_return_type;
            }

            token_t get_name() const
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

            const params_t& get_param_declarations() const
            {
                return m_params;
            }

            const block_node& get_body() const
            {
                return *m_body;
            }

            void visit(ast_node_visitor &visitor) const override
            {
                visitor.visit(*this);
            }

            source_location begin_location() const override
            {
                return m_return_type.tokens().front().src_range().begin;
            }

            source_location end_location() const override
            {
                return m_body->end_location();
            }

        private:
            type_representation m_return_type;
            token_t m_name;
            token_t m_open_paren;
            params_t m_params;
            token_t m_close_paren;
            std::unique_ptr<block_node> m_body;
        };
}
