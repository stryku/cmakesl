

set(BUILTIN_TOKENS_PROVIDERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(BUILTIN_TOKENS_PROVIDER_GENERATOR_COMMAND
    python3
    ${CMAKESL_SCRIPTS_DIR}/builtin_token_providers_generator.py
        ${CMAKESL_DOC_DIR}/builtin_types
        ${BUILTIN_TOKENS_PROVIDERS_DIR}/builtin_token_providers.hpp
)

file(MAKE_DIRECTORY ${BUILTIN_TOKENS_PROVIDERS_DIR})

add_custom_command(
        OUTPUT  ${BUILTIN_TOKENS_PROVIDERS_DIR}/builtin_token_providers.hpp
        COMMAND ${BUILTIN_TOKENS_PROVIDER_GENERATOR_COMMAND}
)

set(SEMA_SOURCES
    add_subdirectory_handler.hpp
    builtin_function_kind.hpp
    builtin_sema_context.cpp
    builtin_sema_context.hpp
    builtin_sema_function.hpp
    builtin_token_provider.cpp
    builtin_token_provider.hpp
    builtin_types_finder.hpp
    factories.cpp
    factories.hpp
    function_lookup_result.hpp
    function_signature.hpp
    homogeneous_generic_type.cpp
    homogeneous_generic_type.hpp
    identifiers_context.hpp
    overload_resolution.cpp
    overload_resolution.hpp
    sema_builder.cpp
    sema_builder.hpp
    sema_builder_ast_visitor.cpp
    sema_builder_ast_visitor.hpp
    sema_context.hpp
    sema_context_impl.cpp
    sema_context_impl.hpp
    sema_function.hpp
    sema_node.cpp
    sema_node.hpp
    sema_node_visitor.hpp
    sema_nodes.hpp
    sema_tree_building_context.hpp
    sema_type.cpp
    sema_type.hpp
    type_builder.cpp
    type_builder.hpp
    type_member_info.hpp
    user_sema_function.hpp
    variable_initialization_checker.cpp
    variable_initialization_checker.hpp

        ${BUILTIN_TOKENS_PROVIDERS_DIR}/builtin_token_providers.hpp
)

add_library(sema "${SEMA_SOURCES}")

target_include_directories(sema
    PRIVATE
        ${CMAKESL_SOURCES_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(sema
    PRIVATE
        ast
)
