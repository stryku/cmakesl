set(EXEC_SOURCES
    exec.cpp
    exec.hpp
    execution_context.hpp
    scope_context.cpp
    scope_context.hpp
    
    stmt/declaration_statement.cpp
    stmt/declaration_statement.hpp
    stmt/return_statement.cpp
    stmt/return_statement.hpp
    stmt/statement.hpp
) 

add_library(exec ${EXEC_SOURCES})
target_include_directories(exec PRIVATE ${CMAKESL_SOURCES_DIR})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX exec FILES ${EXEC_SOURCES})
