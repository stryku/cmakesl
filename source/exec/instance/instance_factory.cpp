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


            std::unique_ptr<instance> instance_factory::create(instance_value_t value) const
            {
                struct type_getter_visitor
                {
                    type_getter_visitor(const ast::ast_context& ctx)
                        : m_ctx{ ctx }
                    {}

                    decltype(auto) operator()(bool) const
                    {
                        return *m_ctx.find_type("bool");
                    }

                    decltype(auto) operator()(inst::int_t) const
                    {
                        return *m_ctx.find_type("int");
                    }

                    decltype(auto) operator()(double) const
                    {
                        return *m_ctx.find_type("double");
                    }

                    decltype(auto) operator()(std::string) const
                    {
                        return *m_ctx.find_type("string");
                    }

                    decltype(auto) operator()(version) const
                    {
                        return *m_ctx.find_type("version");
                    }

                    decltype(auto) operator()(generic_instance_value& gen) const
                    {
                        return gen.get_type();
                    }

                private:
                    const ast::ast_context& m_ctx;
                };

                const auto& t = boost::apply_visitor(type_getter_visitor{ m_ast_ctx }, value);
                return std::make_unique<unnamed_instance>(t, value);
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
