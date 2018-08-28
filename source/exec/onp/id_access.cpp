#include "exec/onp/id_access.hpp"

#include "ast/ast_context.hpp"
#include "exec/instance/instance.hpp"
#include "exec/execution_context.hpp"

namespace cmsl
{
    namespace ast
    {
        class ast_context;
    }

    namespace exec
    {
        namespace onp
        {
            bool id_access::is_function(const ast::ast_context& ctx) const
            {
                if(parent_instance == nullptr)
                {
                    return ctx.find_function(name) != nullptr;
                }
                else
                {
                    return parent_instance->has_function(name);
                }
            }

            inst::instance* id_access::get_instance(execution_context& ctx)
            {
                if(parent_instance == nullptr)
                {
                    return ctx.get_variable(name);
                }
                else
                {
                    return parent_instance->get_member(name);
                }
            }
        }
    }
}
