#pragma once

#include "errors/errors_observer.hpp"
#include "sema/factories.hpp"

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

        class global_executor
        {
        public:
            explicit global_executor(facade::cmake_facade& cmake_facade);

            int execute(source_view source);

        private:
            facade::cmake_facade& m_cmake_facade;
            errors::errors_observer m_errors_observer;
            sema::sema_type_factory m_type_factory;
            sema::sema_function_factory m_function_factory;
            sema::sema_context_factory m_context_factory;
            std::vector<compiled_source> m_compiled_sources;
        };
    }
}
