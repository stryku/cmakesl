set(SEMA_SOURCES
        sema_builder.cpp sema_builder.hpp sema_node.hpp sema_nodes.cpp sema_nodes.hpp sema_node_visitor.hpp sema_builder_ast_visitor.cpp sema_builder_ast_visitor.hpp)

add_library(sema ${SEMA_SOURCES})
target_include_directories(sema PRIVATE ${CMAKESL_SOURCES_DIR})

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX sema FILES ${SEMA_SOURCES})
endif()
