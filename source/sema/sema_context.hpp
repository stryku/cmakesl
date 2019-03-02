#pragma once

#include "sema/sema_type.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace sema
    {
        class sema_function;
        class sema_type;

        // Todo: change name and move to its own file
        class sema_context_interface
        {
        public:
            enum class context_type
            {
                namespace_,
                class_
            };

            virtual ~sema_context_interface() = default;

            // Todo: add_* probably should be moved to a builder or so.
            virtual void add_function(const sema_function& function) = 0;
            virtual void add_type(const sema_type& type) = 0;
            virtual const sema_type* find_type(cmsl::string_view name) const = 0;
            virtual const sema_type* find_type_in_this_scope(cmsl::string_view name) const = 0;
            virtual const sema_function* find_function(cmsl::string_view name) const = 0;
            virtual const sema_function* find_function_in_this_scope(cmsl::string_view name) const = 0;
            virtual context_type type() const = 0;
        };

        class sema_context : public sema_context_interface
        {
        public:
            explicit sema_context(const sema_context_interface* parent = nullptr, context_type context_type = context_type::namespace_);

            void add_function(const sema_function& function) override;
            void add_type(const sema_type& type) override;
            const sema_type* find_type(cmsl::string_view name) const override;
            const sema_type* find_type_in_this_scope(cmsl::string_view name) const override;
            const sema_function* find_function(cmsl::string_view name) const override;
            const sema_function* find_function_in_this_scope(cmsl::string_view name) const override;
            virtual context_type type() const override;

        private:
            const sema_context_interface* m_parent;
            std::vector<const sema_function*> m_functions;
            std::vector<const sema_type*> m_types;
            context_type m_context_type;
        };
    }
}
