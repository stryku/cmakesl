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
            static const auto list_type_ast_ctx = create_list_type_ast_ctx(ctx, value_type);
            return std::make_unique<list_type>(name, list_type_ast_ctx.get(), value_type);
        }

        std::unique_ptr<ast_context> generic_type_factory::create_list_type_ast_ctx(const ast_context& parent, const type& value_type)
        {
            auto ctx = std::make_unique<ast_context>(&parent) ;

            using fun_kind_t = fundamental_function::fundamental_function_kind;

            const auto function_kinds = {
                fun_kind_t::size,
                fun_kind_t::empty
            };

            for(const auto kind : function_kinds)
            {
                fundamental_function::params_declaration_t params{};
                auto fun = std::make_unique<fundamental_function>(kind, params);
                ctx->add_function(std::move(fun));
            }

            // push_back(val)
            {
                fundamental_function::params_declaration_t params{
                    parameter_declaration{ &value_type, lexer::token::token{ lexer::token::token_type::identifier} }
                };

                auto fun = std::make_unique<fundamental_function>(fun_kind_t::push_back, std::move(params));
                ctx->add_function(std::move(fun));
            }

            return std::move(ctx);
        }
    }
}
