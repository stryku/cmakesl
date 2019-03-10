set(SEMA_SOURCES
        sema_builder.cpp sema_builder.hpp sema_node.hpp sema_nodes.cpp sema_nodes.hpp sema_node_visitor.hpp sema_builder_ast_visitor.cpp sema_builder_ast_visitor.hpp identifiers_context.hpp sema_type.hpp sema_context.hpp user_sema_function.hpp sema_function_builder.hpp sema_type.cpp sema_context.cpp sema_function.hpp builtin_sema_context.cpp builtin_sema_context.hpp type_member_info.hpp type_builder.cpp type_builder.hpp function_signature.hpp factories.cpp factories.hpp builtin_sema_function.cpp builtin_sema_function.hpp builtin_function_kind.hpp function_lookup_result.hpp overload_resolution.cpp overload_resolution.hpp)

add_library(sema ${SEMA_SOURCES})
target_include_directories(sema PRIVATE ${CMAKESL_SOURCES_DIR})
target_link_libraries(sema PRIVATE ast)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX sema FILES ${SEMA_SOURCES})
endif()
