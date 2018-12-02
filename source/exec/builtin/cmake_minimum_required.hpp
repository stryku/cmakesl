#pragma once

#include "cmake_facade.hpp"
#include "exec/builtin/evaluatable_function.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instances_holder;
        }

        namespace builtin
        {
            class cmake_minimum_required : public evaluatable_function
            {
            public:
                cmake_minimum_required(facade::cmake_facade& cmake_facade, inst::instances_holder& instances, facade::cmake_facade::version version);

                inst::instance* evaluate() override;

            private:
                facade::cmake_facade& m_facade;
                const facade::cmake_facade::version m_version;
            };
        }
    }
}
