#pragma once

#include "errors/errors_observer.hpp"
#include "sema/factories.hpp"
#include "sema/add_subdirectory_semantic_handler.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace facade
    {
        class cmake_facade;
    }

    namespace exec
    {
        class compiled_source;

        class global_executor : public sema::add_subdirectory_semantic_handler
        {
        public:
            explicit global_executor(facade::cmake_facade& cmake_facade);
            ~global_executor();

            int execute(std::string source);

            const sema::sema_function* handle_add_subdirectory(cmsl::string_view name, const std::vector<const sema::expression_node*>& params) override;

        private:
            facade::cmake_facade& m_cmake_facade;
            errors::errors_observer m_errors_observer;
            sema::sema_type_factory m_type_factory;
            sema::sema_function_factory m_function_factory;
            sema::sema_context_factory m_context_factory;
            std::vector<std::string> m_sources;
            std::vector<std::unique_ptr<compiled_source>> m_compiled_sources;
        };
    }
}
