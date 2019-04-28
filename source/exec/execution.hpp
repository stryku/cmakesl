#pragma once

#include <sema/user_sema_function.hpp>
#include "sema/sema_nodes.hpp"

#include "exec/identifiers_context.hpp"
#include "exec/instance/instance.hpp"
#include "exec/expression_evaluation_context.hpp"
#include "exec/function_caller.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/expression_evaluation_visitor.hpp"
#include "exec/builtin_function_caller2.hpp"
#include "sema/builtin_sema_function.hpp"
#include "exec/instance/instance_factory.hpp"

#include "cmake_facade.hpp"

namespace cmsl::exec
{
        class execution : public identifiers_context, public function_caller
        {
        public:
            explicit execution(facade::cmake_facade& cmake_facade);

            // Todo: consider returning a reference
            // Todo: Call parameters should be a vector of std::unique_ptrs, not raw pointers. Thanks to that, we'd avoid unnecessary instances copies.
            std::unique_ptr<inst::instance> call(const sema::sema_function& fun,
                                 const std::vector<inst::instance*>& params,
                                 inst::instances_holder_interface& instances) override;

            // Todo: consider returning a reference
            std::unique_ptr<inst::instance> call_member(inst::instance& class_instance,
                                        const sema::sema_function& fun,
                                        const std::vector<inst::instance*>& params,
                                        inst::instances_holder_interface& instances) override;

            inst::instance* lookup_identifier(cmsl::string_view identifier) override;

        private:
            void execute_block(const sema::block_node& block);
            void execute_variable_declaration(const sema::variable_declaration_node& node);
            void execute_node(const sema::sema_node& node);

            bool returning_from_function() const;

            const sema::sema_context_interface& current_context() const;

            void enter_function_scope(const sema::sema_function& fun, const std::vector<inst::instance*>& params);
            void enter_function_scope(const sema::sema_function& fun, inst::instance& class_instance, const std::vector<inst::instance*>& params);
            void leave_function_scope();

            std::unique_ptr<inst::instance> execute_infix_expression(const sema::sema_node& node);

        private:
            struct callstack_frame
            {
                const sema::sema_function& fun;
                execution_context exec_ctx;
            };

            facade::cmake_facade& m_cmake_facade;
            std::unique_ptr<inst::instance> m_function_return_value;
            std::stack<callstack_frame> m_callstack;
        };
}
