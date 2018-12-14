#include "exec/builtin/builtin_function_caller.hpp"

#include "ast/builtin_function.hpp"
#include "builtin_function_visitors.hpp"
#include "exec/instance/instances_holder.hpp"
#include "ast/type.hpp"
#include "exec/builtin/container.hpp"
#include "exec/builtin/cmake_minimum_required.hpp"
#include "cmake_facade.hpp" // todo extract version to its own file
#include "exec/instance/instance_value_getter.hpp"

#include <vector>

namespace cmsl
{
    namespace exec
    {
        namespace builtin
        {
            builtin_function_caller::builtin_function_caller(inst::instances_holder_interface &instances, facade::cmake_facade& cmake_facade)
                : m_instances{instances}
                , m_facade{ cmake_facade }
            {}

            inst::instance *builtin_function_caller::call_member_function(inst::instance *class_instance,
                                                                          cmsl::string_view fun_name,
                                                                          const std::vector<inst::instance *> &parameters)
            {
                const auto fun_ptr = class_instance->get_function(fun_name);

                if (auto fun = dynamic_cast<const ast::builtin_function *>(fun_ptr))
                {
                    return call_member_function(class_instance, *fun, parameters);
                }

                CMSL_UNREACHABLE("Not a builtin call instance type.");
                return nullptr;
            }

            void
            builtin_function_caller::call_version_ctor(inst::instance& version_instance, std::vector<inst::instance *> parameters)
            {
                const auto major = parameters[0];
                const auto minor = parameters[1];
                const auto patch = parameters[2];
                const auto tweak = parameters[3];

                auto major_member = version_instance.get_member("major");
                major_member->assign(major->get_value());

                auto minor_member = version_instance.get_member("minor");
                minor_member->assign(minor->get_value());

                auto patch_member = version_instance.get_member("patch");
                patch_member->assign(patch->get_value());

                auto tweak_member = version_instance.get_member("tweak");
                tweak_member->assign(tweak->get_value());
            }

            inst::instance *
            builtin_function_caller::call_push_back(inst::instance *class_instance, std::vector<inst::instance *> parameters)
            {
                auto &val = class_instance->get_value_ref();
                const auto param = parameters[0];
                auto visitor = push_back_visitor{*param};
                visitor.visit(val);
                return m_instances.create_void();
            }

            inst::instance *
            builtin_function_caller::call(ff_kind_t fun, std::vector<inst::instance *> parameters)
            {
                switch (fun)
                {
                    default:
                        CMSL_UNREACHABLE("Unexpected member function call.");
                }
            }

            inst::instance *
            builtin_function_caller::call_member_function(inst::instance *class_instance, const ast::builtin_function &fun, const std::vector<inst::instance *> &parameters)
            {
                switch (fun.get_fundamental_fun_kind())
                {
                    case ast::builtin_function_kind::size:
                    {
                        auto cont = container{ *class_instance, m_instances };
                        return cont.size();
                    }
                    case ast::builtin_function_kind::empty:
                    {
                        auto cont = container{ *class_instance, m_instances };
                        return cont.empty();
                    }
                    case ast::builtin_function_kind::push_back:
                    {
                        return call_push_back(class_instance, parameters);
                    }
                    case ast::builtin_function_kind::version_ctor:
                    {
                        call_version_ctor(*class_instance, parameters);
                        return class_instance;
                    }
                    case ast::builtin_function_kind::project_ctor:
                    {
                        call_project_ctor(*class_instance, parameters);
                        return class_instance;
                    }
                    case ast::builtin_function_kind::project_add_executable:
                    {
                        return call_project_add_executable(*class_instance, parameters);
                    }

                    default:
                        CMSL_UNREACHABLE("Unknown member function call.");
                }
            }

            void
            builtin_function_caller::call_project_ctor(inst::instance &instance, std::vector<inst::instance *> parameters)
            {
                const auto name_param = parameters[0];
                auto name = boost::get<std::string>(name_param->get_value());
                auto name_member = instance.get_member("name");
                m_facade.register_project(name);
                name_member->assign(std::move(name));
            }

            inst::instance*
            builtin_function_caller::call_project_add_executable(inst::instance &instance, std::vector<inst::instance *> parameters)
            {
                const auto to_vector = [](const auto& list)
                {
                    std::vector<std::string> result;
                    std::transform(list.begin(), list.end(), std::back_inserter(result),
                                  [](const auto& value_instance)
                                  {
                                      return boost::get<std::string>(value_instance->get_value_cref());
                                  });

                    return result;
                };

                const auto& executable_name_param = *parameters[0];
                const auto executable_name = boost::get<std::string>(executable_name_param.get_value_cref());

                const auto& sources_list_param = *parameters[1];
                const auto& source_list_instance_value = sources_list_param.get_value_cref();
                const auto& generic_val = boost::get<inst::generic_instance_value>(source_list_instance_value);
                const auto sources_list = generic_val.apply_const(to_vector);

                m_facade.add_executable(executable_name,sources_list);
            }
        }
    }
}
