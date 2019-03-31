#pragma once

#include "common/string.hpp"

namespace cmsl
{
    class source_view;

    namespace facade
    {
        class cmake_facade;
    }
    
    namespace exec
    {
        class source_executor
        {
        public:
            explicit source_executor(facade::cmake_facade& f);
            int execute(cmsl::string_view source);
            int execute2_old(source_view source);
            int execute2_old(cmsl::string_view source_path, cmsl::string_view source);

        private:
            facade::cmake_facade& m_cmake_facade;
        };
    }
}
