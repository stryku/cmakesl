#include "exec/instance_factory.hpp"

#include "ast/ast_context.hpp"
#include "exec/instance.hpp"
#include "exec/named_instance.hpp"

namespace cmsl
{
    namespace exec
    {
        instance_factory::instance_factory(const ast::ast_context& ast_ctx, execution_context& exec_ctx)
            : m_ast_ctx{ ast_ctx }
            , m_exec_ctx{ exec_ctx }
        {}


        const ast::type& instance_factory::get_int_type() const
        {
            return *m_ast_ctx.find_type("int");
        }

        std::unique_ptr<instance> instance_factory::create(int value) const
        {
            return std::make_unique<instance>(get_int_type(), value);
        }

        std::unique_ptr<instance> instance_factory::create(cmsl::string_view name) const
        {
            return std::make_unique<named_instance>(instance::kind::fundamental, get_int_type(), name, m_exec_ctx);
        }

        std::unique_ptr<instance> instance_factory::create(cmsl::string_view name, int value) const
        {
            auto inst = create(name);
            inst->assign(value);
            return std::move(inst);
        }
    }
}
