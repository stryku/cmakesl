set(DECL_SEMA_SOURCES
    builtin_decl_namespace_context.cpp
    builtin_decl_namespace_context.hpp
    component_creation_sema_function.hpp
    declarative_import_handler.hpp
    sema_builder_ast_visitor.cpp
    sema_builder_ast_visitor.hpp
    sema_node_visitor.hpp
    sema_nodes.hpp
)

add_library(decl_sema "${DECL_SEMA_SOURCES}")

target_include_directories(decl_sema
    PRIVATE
        ${CMAKESL_SOURCES_DIR}
        ${CMAKESL_GENERATED_PARENT_DIR}
)

target_link_libraries(decl_sema
    PUBLIC
        decl_ast
        sema
)

target_compile_options(decl_sema
    PRIVATE
        ${CMAKESL_ADDITIONAL_COMPILER_FLAGS}
)
