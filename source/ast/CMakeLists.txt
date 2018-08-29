set(AST_SOURCES
    ast_builder.cpp
    ast_builder.hpp 
    ast_context.cpp
    ast_context.hpp
    block_node.cpp
    block_node.hpp
    builtin_ast_context.cpp
    builtin_ast_context.hpp
    class_node.cpp
    class_node.hpp
    class_type.cpp
    class_type.hpp
    conditional_node.cpp
    conditional_node.hpp
    ast_node_type.hpp
    ast_node.cpp
    ast_node.hpp 
    declaration_node.cpp
    declaration_node.hpp
    function_node.cpp
    function_node.hpp
    if_else_node.cpp
    if_else_node.hpp
    integer_constant.hpp
    integral_constant.hpp
    member_declaration.hpp
    infix_node.cpp
    infix_node.hpp
    parameter_declaration.hpp
    parser.cpp
    parser.hpp
    return_node.cpp
    return_node.hpp
    type.cpp
    type.hpp
        type_conversion_checker.cpp
        type_conversion_checker.hpp
    type_kind.cpp
    type_kind.hpp
    while_node.cpp
    while_node.hpp
)

add_library(ast ${AST_SOURCES})
target_include_directories(ast PRIVATE ${CMAKESL_SOURCES_DIR})

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX ast FILES ${AST_SOURCES})
endif()
