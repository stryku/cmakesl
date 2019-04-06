#pragma once

#include "ast/function.hpp"
#include "ast/block_node.hpp"
#include "ast/type_representation.hpp"
#include "ast/ast_node_visitor.hpp" // todo: to cpp

#include "lexer/token/token.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class user_function_node2 : public ast_node
        {
        public:
            using params_t = std::vector<param_declaration>;

            user_function_node2(type_representation return_type,
                                lexer::token::token name,
                                params_t params,
                                std::unique_ptr<block_node> body)
                    : m_return_type{ std::move(return_type) }
                    , m_name{ name }
                    , m_params{ std::move(params) }
                    , m_body{ std::move(body) }
            {}

            type_representation get_return_type_reference() const
            {
                return m_return_type;
            }

            lexer::token::token get_name() const
            {
                return m_name;
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

        private:
            type_representation m_return_type;
            lexer::token::token m_name;
            params_t m_params;
            std::unique_ptr<block_node> m_body;
        };
    }
}
