#include "instance_factory.hpp"

#include "exec/instance/complex_unnamed_instance.hpp"
#include "exec/instance/simple_unnamed_instance.hpp"
#include "exec/instance/instance_reference.hpp"
#include "common/assert.hpp"
#include "sema/sema_context.hpp"
#include "sema/builtin_types_finder.hpp"

namespace cmsl::exec::inst
{
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
                        case instance_value_variant::which_t::library:
                        {
                            return finder.find_library();
                        }
                        case instance_value_variant::which_t::executable:
                        {
                            return finder.find_executable();
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
