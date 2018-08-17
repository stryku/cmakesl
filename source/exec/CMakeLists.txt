set(EXEC_SOURCES
    exec.cpp
    exec.hpp
    execution_context.cpp
    execution_context.hpp
    fundamental_instance.cpp
    fundamental_instance.hpp
    instance.cpp
    instance.hpp
    instance_factory.cpp
    instance_factory.hpp
    named_instance.cpp
    named_instance.hpp
    scope_context.cpp
    scope_context.hpp
    
    stmt/declaration_statement.cpp
    stmt/declaration_statement.hpp
    stmt/infix_statement.cpp
    stmt/infix_statement.hpp
    stmt/onp_executor.cpp
    stmt/onp_executor.hpp
    stmt/infix_to_onp.cpp
    stmt/infix_to_onp.hpp
    stmt/return_statement.cpp
    stmt/return_statement.hpp
    stmt/statement.hpp
) 

add_library(exec ${EXEC_SOURCES})
target_include_directories(exec PRIVATE ${CMAKESL_SOURCES_DIR})


if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX exec FILES ${EXEC_SOURCES})
endif()

