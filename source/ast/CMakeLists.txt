set(AST_SOURCES
    ast_node.cpp
    ast_node.hpp 
)

add_library(ast ${AST_SOURCES})
target_include_directories(ast PRIVATE ${CMAKESL_SOURCES_DIR})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX ast FILES ${AST_SOURCES})
