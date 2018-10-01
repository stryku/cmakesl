#include "ast/ast_context.hpp"

#include "common/algorithm.hpp"

namespace cmsl
{
    namespace ast
    {
        ast_context_impl::ast_context_impl(const ast_context* parent)
            : m_parent{ parent }
        {}

        void ast_context_impl::add_type(std::unique_ptr<type> t)
        {
            if (cmsl::contains(m_types, t))
            {
                return;
            }

            const auto &tt = *t;
            m_types.emplace_back(std::move(t));
        }

        const type* ast_context_impl::find_type(cmsl::string_view name) const
        {
            const auto it = std::find_if(std::begin(m_types), std::end(m_types),
                                         [name](const auto& t)
            {
                return t->get_name() == name;
            });

            if (it == std::end(m_types))
            {
                if (m_parent == nullptr)
                {
                    return nullptr;
                }

                return m_parent->find_type(name);
            }

            return it->get();
        }

        void ast_context_impl::add_function(std::unique_ptr<function> fun)
        {
            m_functions.emplace_back(std::move(fun));
        }

        const function* ast_context_impl::find_function(cmsl::string_view name) const
        {
            const auto it = std::find_if(std::begin(m_functions), std::end(m_functions),
                                         [name](const auto& t)
            {
                return t->get_name() == name;
            });

            if (it == std::end(m_functions))
            {
                if (m_parent == nullptr)
                {
                    return nullptr;
                }

                return m_parent->find_function(name);
            }

            return it->get();
        }
    }
}
