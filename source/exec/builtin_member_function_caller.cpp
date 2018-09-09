#include "exec/builtin_member_function_caller.hpp"

#include "ast/fundamental_function.hpp"
#include "exec/builtin_function_visitors.hpp"
#include "exec/instance/instances_holder.hpp"

namespace cmsl
{
    namespace exec
    {
        builtin_member_function_caller::builtin_member_function_caller(inst::instances_holder &instances)
            : m_instances{ instances }
        {}

        inst::instance* builtin_member_function_caller::call(inst::instance *class_instance,
                                                             cmsl::string_view fun_name,
                                                             std::vector<inst::instance *> parameters)
        {
            const auto fun_ptr = class_instance->get_function(fun_name);

            using ff_kind_t = ast::fundamental_function::fundamental_function_kind;

            if(auto fun = dynamic_cast<const ast::fundamental_function*>(fun_ptr))
            {
                switch(fun->get_fundamental_fun_kind())
                {
                    case ff_kind_t::size:
                    {
                        auto& val = class_instance->get_value_ref();
                        const auto result = size_visitor{}.visit(val);
                        return m_instances.create(result);
                    }
                    case ff_kind_t::empty:
                    {
                        auto& val = class_instance->get_value_ref();
                        const auto result = empty_visitor{}.visit(val);
                        return m_instances.create(result);
                    }
                    case ff_kind_t::push_back:
                    {
                        auto& val = class_instance->get_value_ref();
                        const auto param = parameters[0];
                        auto visitor = push_back_visitor{ *param };
                        visitor.visit(val);
                        return m_instances.create(true); // todo handle void return type. this true is to return any instance
                    }
                    case ff_kind_t::version_ctor:
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

                        return m_instances.create(class_instance->get_type(), std::move(members));
                    }
                }
            }
        }
    }
}
