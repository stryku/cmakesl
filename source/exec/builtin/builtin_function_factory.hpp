#pragma once

#include <memory>
#include <vector>
#include <ast/builtin_function_kind.hpp>

namespace cmsl
{
    namespace facade
    {
        class cmake_facade;
    }

    namespace ast
    {
        enum class builtin_function_kind;
        class ast_context;
    }

    namespace exec
    {
        namespace inst
        {
            class instance;
            class instances_holder;
        }

        namespace builtin
        {
            class evaluatable_function;

            class builtin_function_factory
            {
            public:
                explicit builtin_function_factory(facade::cmake_facade& cmake_facade);

                std::unique_ptr<evaluatable_function> create(const ast::ast_context& ast_ctx,
                                                             inst::instances_holder& instances,
                                                             ast::builtin_function_kind kind,
                                                             const std::vector<inst::instance*>& params) const;
                std::unique_ptr<evaluatable_function> create_member(inst::instance& class_instance,
                                                                    inst::instances_holder& instances,
                                                                    ast::builtin_function_kind kind,
                                                                    const std::vector<inst::instance*>& params) const;

            private:
                std::unique_ptr<evaluatable_function> create_cmake_minimum_required(inst::instances_holder& instances, const std::vector<inst::instance*>& params) const;
                std::unique_ptr<evaluatable_function> create_push_back(inst::instance& class_instance,
                                                                       inst::instances_holder& instances,
                                                                       const std::vector<inst::instance*>& params) const;

            private:
                facade::cmake_facade& m_facade;
            };
        }
    }
}



