set(DECL_AST_SOURCES
    ast_node.hpp
    ast_nodes.hpp
    parse_errors_observer.hpp
    parser.cpp
    parser.hpp
)

add_library(decl_ast "${DECL_AST_SOURCES}")

target_include_directories(decl_ast
    PRIVATE
        ${CMAKESL_SOURCES_DIR}
)

target_link_libraries(decl_ast
    PUBLIC
        parse
)

target_compile_options(decl_ast
    PRIVATE
        ${CMAKESL_ADDITIONAL_COMPILER_FLAGS}
)
