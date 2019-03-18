#pragma once

#include "sema/sema_type.hpp"

namespace cmsl
{
    namespace sema
    {
        class homogeneous_generic_type : public sema_type
        {
        public:
            explicit homogeneous_generic_type(const sema_context_interface& ctx,
                                              ast::type_representation name,
                                              const sema_type& value_type);

            const sema_type& value_type() const;

        private:
            const sema_type& m_value_type;
        };
    }
}
