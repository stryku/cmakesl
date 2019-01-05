#pragma once

#include "lexer/token/token.hpp"

#include "ast/type.hpp"

namespace cmsl
{
    namespace sema
    {
        // Todo: exists only for test purpose. Consider removing it and using shadow mocking.
        class identifiers_context
        {
        protected:
            using token_t = lexer::token::token;
            using id_map_t = std::unordered_map<token_t, const ast::type*>;

        public:
            virtual ~identifiers_context() = default;

            virtual void register_identifier(token_t name, const ast::type* ty) = 0;
            virtual const ast::type* type_of(cmsl::string_view name) const = 0;
            virtual void enter_ctx() = 0;
            virtual void leave_ctx() = 0;
        };

        // Todo: consider renaming
        class identifiers_context_impl : public identifiers_context
        {
        private:
            using token_t = lexer::token::token;
            using id_map_t = std::unordered_map<token_t, const ast::type*>;

        public:
            void register_identifier(token_t name, const ast::type* ty) override
            {
                // Todo: handle empty
                auto& ctx = m_contextes.back();
                ctx[name] = ty;
            }

            const ast::type* type_of(cmsl::string_view name) const override
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

            void enter_ctx() override
            {
                m_contextes.emplace_back(id_map_t{});
            }

            void leave_ctx() override
            {
                m_contextes.pop_back();
            }

        private:
            std::vector<id_map_t> m_contextes;
        };
    }
}
