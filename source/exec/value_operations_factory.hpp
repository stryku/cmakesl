#pragma once

#include "ast/type.hpp"
#include "exec/builtin_types_operations.hpp"

namespace cmsl
{
    namespace exec
    {
        class value_operations_factory
        {
        public:
            std::unique_ptr<value_operations> create(const ast::type& ty)
            {
                switch(ty.get_kind())
                {
                    case ast::type_kind::k_void:
                    {
                        return nullptr;
                    }
                    case ast::type_kind::k_int:
                    {
                        return std::make_unique<int_operations>();
                    }
                    case ast::type_kind::k_double:break;
                    case ast::type_kind::k_bool:break;
                    case ast::type_kind::k_string:break;
                    case ast::type_kind::k_list:break;
                    case ast::type_kind::k_version:break;
                    case ast::type_kind::k_project:break;
                    case ast::type_kind::k_user:break;
                }

                return nullptr;
            }
        };
    }
}
