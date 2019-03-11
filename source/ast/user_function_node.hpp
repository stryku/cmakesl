#pragma once

#include "ast/function.hpp"
#include "ast/block_node.hpp"
#include "ast/type_name_reference.hpp"
#include "ast/ast_node_visitor.hpp" // todo: to cpp

#include "lexer/token/token.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class ast_context;
        class type;

        class user_function_node : public function
        {
        public:
            user_function_node(ast_context& ctx,
                               const type& return_type,
                               cmsl::string_view name,
                               std::vector<parameter_declaration> params,
                               std::unique_ptr<block_node> body);

            cmsl::string_view get_name() const override;
            const block_node& get_body() const;
            const type& get_return_type() const;

            const ast_context& get_ast_context() const;

            const std::vector<parameter_declaration>& get_params_declarations() const override;

            const type& get_type() const override;

        private:
            ast_context& m_ast_context;
            const type& m_return_type;
            cmsl::string_view m_name;
            std::vector<parameter_declaration> m_params;
            std::unique_ptr<block_node> m_body;
        };

        class user_function_node2 : public ast_node
        {
        public:
            using params_t = std::vector<param_declaration>;

            user_function_node2(type_name_reference return_type,
                                lexer::token::token name,
                                params_t params,
                                std::unique_ptr<block_node> body)
                    : m_return_type{ return_type }
                    , m_name{ name }
                    , m_params{ std::move(params) }
                    , m_body{ std::move(body) }
            {}

            type_name_reference get_return_type_reference() const
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
            type_name_reference m_return_type;
            lexer::token::token m_name;
            params_t m_params;
            std::unique_ptr<block_node> m_body;
        };
    }
}
