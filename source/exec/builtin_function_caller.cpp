#include <vector>
#include "exec/builtin_function_caller.hpp"

#include "ast/fundamental_function.hpp"
#include "exec/builtin_function_visitors.hpp"
#include "exec/instance/instances_holder.hpp"
#include "ast/type.hpp"
#include "cmake_facade.hpp"

namespace cmsl
{
    namespace exec
    {
        builtin_function_caller::builtin_function_caller(inst::instances_holder &instances)
            : m_instances{ instances }
        {}

        inst::instance* builtin_function_caller::call_member_function(inst::instance *class_instance,
                                                                      cmsl::string_view fun_name,
                                                                      const std::vector<inst::instance *>& parameters)
        {
            const auto fun_ptr = class_instance->get_function(fun_name);

            if(auto fun = dynamic_cast<const ast::fundamental_function*>(fun_ptr))
            {
                return call_member_function(class_instance, *fun, parameters);
            }

            CMSL_UNREACHABLE("Not a fundamental call instance type.");
            return nullptr;
        }

        cmsl::exec::inst::instance *
        cmsl::exec::builtin_function_caller::call_version_ctor(const ast::type &type, std::vector<inst::instance *> parameters)
        {
            const auto major = parameters[0];
            const auto minor = parameters[1];
            const auto patch = parameters[2];
            const auto tweak = parameters[3];

            inst::instance_members_t members;
            members.emplace("major", major->copy());
            members.emplace("minor", minor->copy());
            members.emplace("patch", patch->copy());
            members.emplace("tweak", tweak->copy());

            return m_instances.create(type, std::move(members));
        }

        inst::instance *
        cmsl::exec::builtin_function_caller::call_push_back(inst::instance *class_instance, std::vector<inst::instance *> parameters)
        {
            auto& val = class_instance->get_value_ref();
            const auto param = parameters[0];
            auto visitor = push_back_visitor{ *param };
            visitor.visit(val);
            return m_instances.create(true); // todo handle void return type. this true is to return any instance
        }

        inst::instance *
        builtin_function_caller::call(builtin_function_caller::ff_kind_t fun, std::vector<inst::instance *> parameters)
        {
            switch(fun)
            {
                case ast::builtin_function_kind::cmake_minimum_required:
                {
                    const auto current_cmake_version = facade::cmake_facade{}.get_cmake_version();
                    return m_instances.create(true); // todo handle void return type. this true is to return any instance
                }

                default:
                    CMSL_UNREACHABLE("Unexpected member function call.");
            }
        }

        inst::instance *
        builtin_function_caller::call_member_function(inst::instance *class_instance, const ast::fundamental_function& fun, const std::vector<inst::instance *>& parameters)
        {
            switch(fun.get_fundamental_fun_kind())
            {
                case ast::builtin_function_kind::size:
                {
                    auto& val = class_instance->get_value_ref();
                    const auto result = size_visitor{}.visit(val);
                    return m_instances.create(result);
                }
                case ast::builtin_function_kind::empty:
                {
                    auto& val = class_instance->get_value_ref();
                    const auto result = empty_visitor{}.visit(val);
                    return m_instances.create(result);
                }
                case ast::builtin_function_kind::push_back:
                {
                    return call_push_back(class_instance, parameters);
                }
                case ast::builtin_function_kind::version_ctor:
                {
                    return call_version_ctor(class_instance->get_type(), parameters);
                }
            }
        }
    }
}
