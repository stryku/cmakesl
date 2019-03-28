#include "instance_factory.hpp"

#include "ast/ast_context.hpp"
#include "exec/instance/complex_unnamed_instance.hpp"
#include "exec/instance/simple_unnamed_instance.hpp"
#include "exec/instance/instance_reference.hpp"
#include "common/assert.hpp"
#include "sema/sema_context.hpp"
#include "sema/builtin_types_finder.hpp"

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

                const auto& t = [&value, &ast_ctx]() -> const ast::type&
                {
                    switch(value.which())
                    {

                        // Todo: store found types somewhere
                        case instance_value_variant::which_t::bool_: return *ast_ctx.find_type("bool");
                        case instance_value_variant::which_t::int_: return *ast_ctx.find_type("int");
                        case instance_value_variant::which_t::double_: return *ast_ctx.find_type("int");
                        case instance_value_variant::which_t::string: return *ast_ctx.find_type("int");
                        case instance_value_variant::which_t::generic:
                        {
                            return value.get_generic_cref().get_type();
                        }
                    }
                }();

                return std::make_unique<simple_unnamed_instance>(t, value);
            }

            std::unique_ptr<instance> instance_factory::create(cmsl::string_view name, execution_context &exec_ctx) const
            {
                return std::make_unique<instance_reference>(name, exec_ctx);
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

            std::unique_ptr<instance> instance_factory2::create(instance_value_t value, const sema::sema_context_interface &ctx) const
            {
                const auto type_getter = [&value, &ctx]() -> const sema::sema_type&
                {
                    sema::builtin_types_finder finder{ ctx };
                    switch(value.which())
                    {
                        // Todo: cache types, don't find it over every time.
                        case instance_value_variant::which_t::bool_:
                        {
                            return finder.find_bool();
                        }
                        case instance_value_variant::which_t::int_:
                        {
                            return finder.find_int();
                        }
                        case instance_value_variant::which_t::double_:
                        {
                            return finder.find_double();
                        }
                        case instance_value_variant::which_t::string:
                        {
                            return finder.find_string();
                        }
                        case instance_value_variant::which_t::target:
                        {
                            return finder.find_target();
                        }
                        case instance_value_variant::which_t::list:
                        {
                            return finder.find_string();
                        }
                        default:
                            CMSL_UNREACHABLE("Unknown type requested");
                    }
                };

                const auto& type = type_getter();
                return std::make_unique<simple_unnamed_instance>(type, value);
            }

            std::unique_ptr<instance> instance_factory2::create_reference(instance &referenced_instance) const
            {
                return std::make_unique<instance_reference>(referenced_instance);
            }

            std::unique_ptr<instance> instance_factory2::create(const sema::sema_type &type) const
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

            std::unique_ptr<instance> instance_factory2::create(const sema::sema_type &type, instance_value_t value) const
            {
                return std::make_unique<simple_unnamed_instance>(type, std::move(value));
            }
        }
    }
}
