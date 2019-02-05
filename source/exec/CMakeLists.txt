set(EXEC_SOURCES
    context_provider.hpp
    execution.cpp
    execution.hpp
    execution_context.cpp
    execution_context.hpp
        builtin/builtin_function_visitors.hpp
        builtin/builtin_function_caller.cpp
        builtin/builtin_function_caller.hpp
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
        infix/function_call_expression.cpp
        infix/function_call_expression.hpp
        infix/member_function_call_expression.cpp
        infix/member_function_call_expression.hpp
    infix/call_expression.cpp
    infix/call_expression.hpp
    infix/infix_expression_builder.cpp
    infix/infix_expression_builder.hpp
    infix/infix_expression_visitor.hpp
    infix/operator_evaluation_visitor.cpp
    infix/operator_evaluation_visitor.hpp

        instance/conversion_visitor.hpp
    instance/instance.hpp
    instance/instance_converter.cpp
    instance/instance_converter.hpp
    instance/instances_holder.cpp
        instance/instances_holder.hpp
        instance/instances_holder_interface.hpp
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
        builtin/container.cpp builtin/container.hpp builtin/cmake_minimum_required.cpp builtin/cmake_minimum_required.hpp instance/instance_value_getter.hpp builtin/evaluatable_function.hpp builtin/builtin_function_factory.cpp builtin/builtin_function_factory.hpp builtin/evaluatable_member_function.hpp builtin/builtin_member_functions.cpp builtin/builtin_member_functions.hpp builtin/version.cpp builtin/version.hpp execution2.hpp expression_evaluation_visitor.hpp
        expression_evaluation_context.hpp
        builtin_types_operations.hpp value_operations_factory.hpp identifiers_context.hpp instance/instance_value_variant.cpp instance/instance_value_variant.hpp)

add_library(exec ${EXEC_SOURCES})
target_include_directories(exec PRIVATE ${CMAKESL_SOURCES_DIR} ${CMAKESL_FACADE_DIR})
target_link_libraries(exec PRIVATE sema)


if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX exec FILES ${EXEC_SOURCES})
endif()

