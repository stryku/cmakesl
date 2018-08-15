#pragma once

#include "ast/type.hpp"
#include "ast/class_node.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class class_type : public type
        {
        public:
            class_type(std::unique_ptr<class_node> node);

        private:
            std::unique_ptr<class_node> m_node;
        };
    }
}
