#include "ast/class_builder.hpp"

#include "ast/class_type.hpp"
#include "ast/ast_context.hpp"
#include "ast/class_node.hpp"

namespace cmsl
{
    namespace ast
    {
        class_builder::class_builder(ast_context &parent_ast_ctx, cmsl::string_view name)
            : m_ast_ctx{ std::make_unique<ast_context>(&parent_ast_ctx) }
            , m_name{ name }
        {}

        class_builder::~class_builder()
        {}

        std::unique_ptr<class_node> class_builder::build()
        {
            return std::make_unique<class_node>(std::move(m_ast_ctx), m_name, std::move(m_members));
        }

        class_builder& class_builder::with_member(member_declaration member)
        {
            m_members.emplace_back(std::move(member));
            return *this;
        }

        class_builder &class_builder::with_function(std::unique_ptr<function_node> fun)
        {
            m_ast_ctx->add_function(std::move(fun));
            return *this;
        }
    }
}
