set(EXEC_SOURCES
    context_provider.hpp
    exec.cpp
    exec.hpp
    execution_context.cpp
    execution_context.hpp
    fundamental_instance.cpp
    fundamental_instance.hpp
    scope_context.cpp
    scope_context.hpp

    instance/conversion_visitor.hpp
    instance/instance.hpp
    instance/instance_converter.cpp
    instance/instance_converter.hpp
    instance/unnamed_instance.cpp
    instance/unnamed_instance.hpp
    instance/instances_holder.cpp
    instance/instances_holder.hpp
    instance/instance_factory.cpp
    instance/instance_factory.hpp
    instance/instance_value.hpp
    instance/named_instance.cpp
    instance/named_instance.hpp

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

    stmt/builtin_call_size_statement.cpp
    stmt/builtin_call_size_statement.hpp
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

