#include "instance_factory.hpp"

#include "ast/ast_context.hpp"
#include "exec/instance/complex_unnamed_instance.hpp"
#include "exec/instance/simple_unnamed_instance.hpp"
#include "exec/instance/named_instance.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            std::unique_ptr<instance> instance_factory::create(instance_value_t value, const ast::ast_context &ast_ctx) const
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

                    // todo
                    //decltype(auto) operator()(version) const
                    //{
//                        return *m_ctx.find_type("version");
//                    }

                    decltype(auto) operator()(generic_instance_value& gen) const
                    {
                        return gen.get_type();
                    }

                private:
                    const ast::ast_context& m_ctx;
                };

                const auto& t = boost::apply_visitor(type_getter_visitor{ ast_ctx }, value);
                return std::make_unique<simple_unnamed_instance>(t, value);
            }

            std::unique_ptr<instance> instance_factory::create(cmsl::string_view name, execution_context &exec_ctx) const
            {
                return std::make_unique<named_instance>(name, exec_ctx);
            }

            std::unique_ptr<instance> instance_factory::create(cmsl::string_view name, instance_value_t value, execution_context &exec_ctx) const
            {
                auto inst = create(name, exec_ctx);
                inst->assign(value);
                return std::move(inst);
            }

            std::unique_ptr<instance> instance_factory::create(const ast::type& type) const
            {
                if(type.is_complex())
                {
                    return std::make_unique<complex_unnamed_instance>(type);
                }
                else
                {
                    return std::make_unique<simple_unnamed_instance>(type);
                }
            }

            /*std::unique_ptr<instance> instance_factory::create(instance_members_t members) const
            {
                return std::make_unique<complex_unnamed_instance>(std::move(members));
            }*/

            std::unique_ptr<instance>
            instance_factory::create(const ast::type& type, instance_members_t members) const
            {
                return std::make_unique<complex_unnamed_instance>(type, std::move(members));
            }

            //////////////////////////
            contexted_instance_factory::contexted_instance_factory(const ast::ast_context &ast_ctx, execution_context &exec_ctx)
                : m_ast_ctx{ ast_ctx }
                , m_exec_ctx{ exec_ctx }
            {}

            std::unique_ptr<instance> contexted_instance_factory::create(instance_value_t value) const
            {
                return instance_factory{}.create(value, m_ast_ctx);
            }

            std::unique_ptr<instance> contexted_instance_factory::create(cmsl::string_view name) const
            {
                return instance_factory{}.create(name, m_exec_ctx);
            }

            std::unique_ptr<instance> contexted_instance_factory::create(cmsl::string_view name, instance_value_t value) const
            {
                return instance_factory{}.create(name, value, m_exec_ctx);
            }

            std::unique_ptr<instance> contexted_instance_factory::create(const ast::type& type) const
            {
                return instance_factory{}.create(type);
            }

            /*std::unique_ptr<instance> contexted_instance_factory::create(instance_members_t members) const
            {
                return instance_factory{}.create(std::move(members));
            }*/

            std::unique_ptr<instance>
            contexted_instance_factory::create(const ast::type& type, instance_members_t members) const
            {
                return instance_factory{}.create(type, std::move(members));
            }
        }
    }
}
