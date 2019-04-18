#pragma once

#include "ast/ast_node.hpp"

#include <memory>
#include <vector>

namespace cmsl::ast
{
        class conditional_node;
        class block_node;


        class if_else_node : public ast_node
        {
        public:
            struct if_values
            {
                std::optional<token_t> else_kw;
                token_t if_kw;
                std::unique_ptr<conditional_node> conditional;
            };

            struct last_else_value
            {
                token_t else_kw;
                std::unique_ptr<block_node> body;
            };


            using ifs_t = std::vector<if_values>;

            explicit if_else_node(ifs_t m_ifs, std::optional<last_else_value> else_);
            ~if_else_node();

            // Todo: remove get_
            const ifs_t& get_ifs() const;

            std::optional<token_t> else_kw() const;
            const block_node* get_else_body() const;

            void visit(ast_node_visitor &visitor) const override;
            source_location begin_location() const override;
            source_location end_location() const override;

        private:
            ifs_t m_ifs;
            std::optional<last_else_value> m_else;
        };
}
