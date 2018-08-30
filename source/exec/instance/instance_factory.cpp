#include "instance_factory.hpp"

#include "ast/ast_context.hpp"
#include "unnamed_instance.hpp"
#include "named_instance.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            instance_factory::instance_factory(const ast::ast_context &ast_ctx, execution_context &exec_ctx)
                : m_ast_ctx{ ast_ctx }
                , m_exec_ctx{ exec_ctx }
            {}


            const ast::type &instance_factory::get_int_type() const
            {
                return *m_ast_ctx.find_type("int");
            }

            std::unique_ptr<instance> instance_factory::create(instance_value_t value) const
            {
                return std::make_unique<unnamed_instance>(get_int_type(), value);
            }

            std::unique_ptr<instance> instance_factory::create(cmsl::string_view name) const
            {
                return std::make_unique<named_instance>(name, m_exec_ctx);
            }

            std::unique_ptr<instance> instance_factory::create(cmsl::string_view name, instance_value_t value) const
            {
                auto inst = create(name);
                inst->assign(value);
                return std::move(inst);
            }

            std::unique_ptr<instance> instance_factory::create(const ast::type& type) const
            {
                return std::make_unique<unnamed_instance>(type);
            }
        }
    }
}
