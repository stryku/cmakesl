#include "ast/generic_type_factory.hpp"

#include "ast/ast_context.hpp"
#include "ast/list_type.hpp"
#include "ast/builtin_function.hpp"

namespace cmsl
{
    namespace ast
    {
        std::unique_ptr<list_type> generic_type_factory::create_list(const std::string& name, const ast_context& ctx, const type& value_type)
        {
            static const auto list_type_ast_ctx = create_list_type_ast_ctx(ctx, value_type);
            return std::make_unique<list_type>(name, list_type_ast_ctx.get(), value_type);
        }

        std::unique_ptr<ast_context> generic_type_factory::create_list_type_ast_ctx(const ast_context& parent, const type& value_type)
        {
            auto ctx = std::make_unique<ast_context_impl>(&parent) ;

            const auto functions = {
                    std::make_pair(builtin_function_kind::size, "int"),
                    std::make_pair(builtin_function_kind::empty, "bool")
            };

            for(const auto fun_pair : functions)
            {
                const auto kind = fun_pair.first;
                const auto return_type = parent.find_type(fun_pair.second);
                auto fun = std::make_unique<builtin_function>(*return_type, kind, builtin_function::params_declaration_t{});
                ctx->add_function(std::move(fun));
            }

            // push_back(val)
            {
                builtin_function::params_declaration_t params{
                    parameter_declaration{ &value_type, lexer::token::token{ lexer::token::token_type::identifier } }
                };

                auto fun = std::make_unique<builtin_function>(*parent.find_type("void"), builtin_function_kind::push_back, std::move(params));
                ctx->add_function(std::move(fun));
            }

            return std::move(ctx);
        }
    }
}
