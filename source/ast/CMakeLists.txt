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
    class_builder.cpp
    class_builder.hpp
    common_type_finder.cpp
    common_type_finder.hpp
    conditional_node.cpp
    conditional_node.hpp
    ast_node_type.hpp
    ast_node.cpp
    ast_node.hpp 
    declaration_node.cpp
    declaration_node.hpp
    user_function_node.cpp
    user_function_node.hpp
    function.hpp
    builtin_function.hpp
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
    parser2.cpp
    parser2.hpp
    return_node.cpp
    return_node.hpp
    type.cpp
    type.hpp
    type_node.hpp
    type_conversion_check.cpp
    type_conversion_check.hpp
    type_kind.cpp
    type_kind.hpp
    type_operator_support_check.cpp
    type_operator_support_check.hpp
    while_node.cpp
    while_node.hpp
    list_type.cpp
    list_type.hpp
    generic_type_factory.hpp
    generic_type_factory.cpp
    builtin_function_kind.hpp
    variable_declaration_node.hpp
    type_reference.hpp
)

add_library(ast ${AST_SOURCES})
target_include_directories(ast PRIVATE ${CMAKESL_SOURCES_DIR})

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX ast FILES ${AST_SOURCES})
endif()
