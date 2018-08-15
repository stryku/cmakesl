#include "ast/class_type.hpp"
#include "ast/type_kind.hpp"
#include "ast/class_node.hpp"

namespace cmsl
{
    namespace ast
    {
        class_type::class_type(std::unique_ptr<class_node> node)
            : type{ node->get_name(), type_kind::k_user }
            , m_node{ std::move(node) }
        {}
    }
}
