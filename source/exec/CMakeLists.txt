set(EXEC_SOURCES
    context_provider.hpp
    execution.cpp
    execution.hpp
    execution_context.cpp
    execution_context.hpp
    builtin_function_visitors.hpp
    builtin_function_caller.cpp
    builtin_function_caller.hpp
    function_caller.hpp
    fundamental_instance.cpp
    fundamental_instance.hpp
    scope_context.cpp
    scope_context.hpp
    source_executor.cpp
    source_executor.hpp

    infix/infix_expression.hpp
    infix/function_call_evaluator.cpp
    infix/function_call_evaluator.hpp
    infix/fundamental_value_expression.cpp
    infix/fundamental_value_expression.hpp
    infix/infix_evaluation_context.hpp
    infix/binary_operator_expression.cpp
    infix/binary_operator_expression.hpp
    infix/class_member_access_expression.cpp
    infix/class_member_access_expression.hpp
    infix/id_expression.cpp
    infix/id_expression.hpp
    infix/user_function_call_expression.cpp
    infix/user_function_call_expression.hpp
    infix/user_member_function_call_expression.cpp
    infix/user_member_function_call_expression.hpp
    infix/call_expression.cpp
    infix/call_expression.hpp
    infix/infix_expression_builder.cpp
    infix/infix_expression_builder.hpp
    infix/infix_expression_visitor.hpp

    instance/conversion_visitor.hpp
    instance/instance.hpp
    instance/instance_converter.cpp
    instance/instance_converter.hpp
    instance/instances_holder.cpp
    instance/instances_holder.hpp
    instance/instance_factory.cpp
    instance/instance_factory.hpp
    instance/instance_value.hpp
    instance/named_instance.cpp
    instance/named_instance.hpp
    instance/generic_instance_value.cpp
    instance/generic_instance_value.hpp
    instance/instance_value.cpp
    instance/int_alias.hpp
    instance/instance_members_alias.hpp
    instance/simple_unnamed_instance.cpp
    instance/simple_unnamed_instance.hpp
    instance/complex_unnamed_instance.cpp
    instance/complex_unnamed_instance.hpp

    onp/id_access.cpp
    onp/id_access.hpp
    onp/infix_to_onp.cpp
    onp/infix_to_onp.hpp
    onp/onp_entry.hpp
    onp/onp_executor.cpp
    onp/onp_executor.hpp
    onp/operator_evaluation_visitor.cpp
    onp/operator_evaluation_visitor.hpp
    onp/operator_visitor.cpp
    onp/operator_visitor.hpp

    stmt/declaration_statement.cpp
    stmt/declaration_statement.hpp
    stmt/if_else_statement.cpp
    stmt/if_else_statement.hpp
    stmt/infix_statement.cpp
    stmt/infix_statement.hpp
    stmt/return_statement.cpp
    stmt/return_statement.hpp
    stmt/statement.hpp
    stmt/while_statement.cpp
    stmt/while_statement.hpp
        )

add_library(exec ${EXEC_SOURCES})
target_include_directories(exec PRIVATE ${CMAKESL_SOURCES_DIR})


if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX exec FILES ${EXEC_SOURCES})
endif()

