#include "ast/generic_type_factory.hpp"

#include "ast/ast_context.hpp"
#include "ast/list_type.hpp"
#include "ast/fundamental_function.hpp"

namespace cmsl
{
    namespace ast
    {
        std::unique_ptr<list_type> generic_type_factory::create_list(const std::string& name, const ast_context& ctx, const type& value_type)
        {
            static const auto list_type_ast_ctx = create_list_type_ast_ctx(ctx);
            return std::make_unique<list_type>(name, list_type_ast_ctx.get(), value_type);
        }

        std::unique_ptr<ast_context> generic_type_factory::create_list_type_ast_ctx(const ast_context& parent)
        {
            auto ctx = std::make_unique<ast_context>(&parent) ;

            auto fun = std::make_unique<fundamental_function>(fundamental_function::fundamental_function_kind::size);
            ctx->add_function(std::move(fun));

            return std::move(ctx);
        }
    }
}
