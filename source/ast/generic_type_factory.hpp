#pragma once

#include <memory>
#include <string>

namespace cmsl
{
    namespace ast
    {
        class ast_context;
        class type;
        class list_type;

        class generic_type_factory
        {
        public:
            std::unique_ptr<list_type> create_list(const std::string& name, const ast_context& ctx, const type& value_type);

        private:
            std::unique_ptr<ast_context> create_list_type_ast_ctx(const ast_context& parent);
        };
    }
}
