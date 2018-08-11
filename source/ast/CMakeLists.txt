set(AST_SOURCES
    ast_builder.cpp
    ast_builder.hpp 
    ast_context.cpp
    ast_context.hpp
    block_node.cpp
    block_node.hpp
    builtin_ast_context.cpp
    builtin_ast_context.hpp
    ast_node_type.hpp
    ast_node.cpp
    ast_node.hpp 
    declaration_node.cpp
    declaration_node.hpp
    function_node.cpp
    function_node.hpp
    integral_constant.hpp
    infix_node.cpp
    infix_node.hpp
    parameter_declaration.hpp
    parser.cpp
    parser.hpp
    return_node.cpp
    return_node.hpp
    type.cpp
    type.hpp
    type_kind.cpp
    type_kind.hpp
)

add_library(ast ${AST_SOURCES})
target_include_directories(ast PRIVATE ${CMAKESL_SOURCES_DIR})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX ast FILES ${AST_SOURCES})
