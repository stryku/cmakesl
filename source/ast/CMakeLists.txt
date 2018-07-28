set(AST_SOURCES
    ast_builder.cpp
    ast_builder.hpp 
    ast_node.cpp
    ast_node.hpp 
    function.hpp
    integral_constant.hpp
    onp_expression.hpp
    parameter_declaration.hpp
    parser.cpp
    parser.hpp
    type.cpp
    type.hpp
    
    expr/binary_operator_expression.cpp
    expr/binary_operator_expression.hpp
    expr/block_expression.cpp
    expr/block_expression.hpp
    expr/constant_expression.cpp
    expr/constant_expression.hpp
    expr/expression.cpp
    expr/expression.hpp 
    expr/onp_expression.cpp
    expr/onp_expression.hpp 
)

add_library(ast ${AST_SOURCES})
target_include_directories(ast PRIVATE ${CMAKESL_SOURCES_DIR})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX ast FILES ${AST_SOURCES})
