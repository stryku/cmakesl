#include "ast/ast_context.hpp"

#include "ast/type.hpp"
#include "common/algorithm.hpp"

namespace cmsl
{
    namespace ast
    {
        ast_context::ast_context(ast_context* parent)
            : m_parent{ parent }
        {}

        void ast_context::add_type(std::unique_ptr<type> t)
        {
            if (cmsl::contains(m_types, t))
            {
                return;
            }

            m_types.emplace_back(std::move(t));
        }

        type* ast_context::find_type(cmsl::string_view name)
        {
            const auto it = std::find_if(std::begin(m_types), std::end(m_types),
                                         [name](const auto t)
            {
                return t->get_name() == name;
            });

            if (it == std::end(m_types))
            {
                return nullptr;
            }

            return it->get();
        }
    }
}
