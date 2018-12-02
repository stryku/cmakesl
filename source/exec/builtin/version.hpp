#pragma once

#include "exec/builtin/evaluatable_function.hpp"

namespace cmsl
{
    namespace ast
    {
        class type;
    }

    namespace exec
    {
        namespace inst
        {
            class instances_holder;
        }

        namespace builtin
        {
            class version_ctor : public evaluatable_function
            {
            public:
                explicit version_ctor(inst::instances_holder& instances,
                                      const ast::type& version_type,
                                      const inst::instance& major,
                                      const inst::instance& minor,
                                      const inst::instance& patch,
                                      const inst::instance& tweak);

                inst::instance* evaluate() override;

            private:
                const ast::type& m_version_type;
                const inst::instance &m_major;
                const inst::instance &m_minor;
                const inst::instance &m_patch;
                const inst::instance &m_tweak;
            };
        }
    }
}
