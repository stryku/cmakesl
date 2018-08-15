#pragma once

#include "ast/ast_node.hpp"
#include "ast/type.hpp"
#include "common/string.hpp"

#include <unordered_map>

namespace cmsl
{
    namespace ast
    {
        class class_node : public ast_node
        {
        public:
            class_node(string_view_map<const type*> m_members);

        private:
            string_view_map<const type*> m_members;
        };
    }
}
