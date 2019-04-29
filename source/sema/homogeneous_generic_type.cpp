#include "sema/homogeneous_generic_type.hpp"
#include "homogeneous_generic_type.hpp"


namespace cmsl::sema
{
        homogeneous_generic_type::homogeneous_generic_type(const sema_context &ctx, ast::type_representation name, const sema_type& value_type)
            : sema_type{ ctx, std::move(name), {}}
            , m_value_type{ value_type }
        {}

        const sema_type &homogeneous_generic_type::value_type() const
        {
            return m_value_type;
        }
}
