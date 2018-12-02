#include "exec/builtin/builtin_function_factory.hpp"

#include "ast/builtin_function_kind.hpp"
#include "ast/ast_context.hpp"
#include "common/assert.hpp"
#include "exec/builtin/cmake_minimum_required.hpp"
#include "exec/instance/instance_value_getter.hpp"
#include "exec/builtin/builtin_member_functions.hpp"
#include "exec/builtin/builtin_function_visitors.hpp"
#include "exec/builtin/version.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace builtin
        {
            builtin_function_factory::builtin_function_factory(facade::cmake_facade &cmake_facade)
                : m_facade{ cmake_facade }
            {}

            std::unique_ptr<evaluatable_function> builtin_function_factory::create(const ast::ast_context& ast_ctx,
                                                                                   inst::instances_holder_interface& instances,
                                                                                   ast::builtin_function_kind kind,
                                                                                   const std::vector<inst::instance*>& params) const
            {
                switch (kind)
                {
                    case ast::builtin_function_kind::cmake_minimum_required:
                    {
                        return create_cmake_minimum_required(instances, params);
                    }
                    case ast::builtin_function_kind::version_ctor:
                    {
                        return std::make_unique<version_ctor>(instances,
                                                              *ast_ctx.find_type("version"),
                                                              *params[0], // major
                                                              *params[1], // minor
                                                              *params[2], // patch
                                                              *params[3]); // tweak
                    }

                    case ast::builtin_function_kind::size:
                    case ast::builtin_function_kind::empty:
                    case ast::builtin_function_kind::push_back:
                    case ast::builtin_function_kind::project_ctor:
                    case ast::builtin_function_kind::project_add_executable:
                    {
                        CMSL_UNREACHABLE("Trying to create a member function as a non member function.");
                        return nullptr;
                    }
                    break;

                    default:
                        CMSL_UNREACHABLE("Unknown builtin function kind passed");
                        return nullptr;
                }
            }

            std::unique_ptr<evaluatable_function>
            builtin_function_factory::create_member(inst::instance &class_instance, inst::instances_holder_interface &instances, ast::builtin_function_kind kind, const std::vector<inst::instance *> &params) const
            {
                switch (kind)
                {
                    case ast::builtin_function_kind::size:
                    {
                        return std::make_unique<size_member_function>(class_instance, instances);
                    }

                    case ast::builtin_function_kind::empty:
                    {
                        return std::make_unique<empty_member_function>(class_instance, instances);
                    }

                    case ast::builtin_function_kind::push_back:
                    {
                        return create_push_back(class_instance, instances, params);
                    }

                    case ast::builtin_function_kind::cmake_minimum_required:
                    {
                        CMSL_UNREACHABLE("Trying to create non member function as member function.");
                        return nullptr;
                    }
                    break;

                    case ast::builtin_function_kind::version_ctor:
                    case ast::builtin_function_kind::project_ctor:
                    case ast::builtin_function_kind::project_add_executable:
                    default:
                        CMSL_UNREACHABLE("Unknown builtin function kind passed");
                        return nullptr;
                }
            }

            std::unique_ptr<evaluatable_function>
            builtin_function_factory::create_cmake_minimum_required(inst::instances_holder_interface& instances, const std::vector<inst::instance *> &params) const
            {
                const auto value_getter = inst::instance_value_getter{};

                const auto version = facade::cmake_facade::version{
                    static_cast<size_t>(value_getter.int_(*params[0])), // major
                    static_cast<size_t>(value_getter.int_(*params[1])), // minor
                    static_cast<size_t>(value_getter.int_(*params[2])), // patch
                    static_cast<size_t>(value_getter.int_(*params[3]))  // tweak
                };

                return std::make_unique<cmake_minimum_required>(m_facade, instances, version);
            }

            // todo it's not safe. Taking a reference to a element of a const ref vector
            std::unique_ptr<evaluatable_function>
            builtin_function_factory::create_push_back(inst::instance &class_instance, inst::instances_holder_interface &instances, const std::vector<inst::instance *> &params) const
            {
                const auto &val = class_instance.get_value_cref();
                const auto& param = *params[0];
                return std::make_unique<push_back_member_function>(class_instance, instances, param);
            }
        }
    }
}
