#pragma once

#include "ast/type.hpp"
#include "type_kind.hpp"

namespace cmsl
{
    namespace ast
    {
        class list_type : public type
        {
        public:
            explicit list_type(std::string name, const ast_context* ast_ctx, const type& value_type);

            const type& m_value_type;

        private:
            std::unique_ptr<ast_context> m_ctx;
        };
    }
}
