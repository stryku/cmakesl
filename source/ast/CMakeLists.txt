set(AST_SOURCES
        block_node.cpp
    block_node.hpp
        class_node.hpp
        conditional_node.cpp
    conditional_node.hpp
        ast_node.hpp
        user_function_node.hpp
        if_else_node.cpp
    if_else_node.hpp
        parameter_declaration.hpp
        parser2.cpp
    parser2.hpp
    return_node.cpp
    return_node.hpp
        type_kind.cpp
    type_kind.hpp
        while_node.cpp
    while_node.hpp
        variable_declaration_node.hpp
        type_representation.hpp
        infix_nodes.hpp translation_unit_node.hpp ast_node_visitor.hpp parser_utils.cpp parser_utils.hpp type_parser.cpp type_parser.hpp parse_errors_observer.hpp type_parsing_result.hpp)

add_library(ast ${AST_SOURCES})
target_include_directories(ast PRIVATE ${CMAKESL_SOURCES_DIR})
target_link_libraries(ast lexer)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX ast FILES ${AST_SOURCES})
endif()
