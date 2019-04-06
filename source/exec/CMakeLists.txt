set(EXEC_SOURCES
    context_provider.hpp
        execution_context.cpp
    execution_context.hpp
        function_caller.hpp
        scope_context.cpp
    scope_context.hpp

        instance/instance.hpp
        instance/instances_holder.cpp
        instance/instances_holder.hpp
        instance/instances_holder_interface.hpp
    instance/instance_factory.cpp
    instance/instance_factory.hpp
    instance/instance_value.hpp
        instance/instance_reference.cpp
        instance/instance_reference.hpp
        instance/instance_value.cpp
    instance/int_alias.hpp
    instance/instance_members_alias.hpp
    instance/simple_unnamed_instance.cpp
    instance/simple_unnamed_instance.hpp
    instance/complex_unnamed_instance.cpp
    instance/complex_unnamed_instance.hpp
        instance/instance_value_getter.hpp execution2.hpp expression_evaluation_visitor.hpp
        expression_evaluation_context.hpp
        identifiers_context.hpp instance/instance_value_variant.cpp instance/instance_value_variant.hpp builtin_function_caller2.cpp builtin_function_caller2.hpp instance/version_value.hpp instance/version_value.cpp instance/list_value.cpp instance/list_value.hpp instance/instance_value_alternative.hpp parameter_alternatives_getter.hpp instance/project_value.cpp instance/project_value.hpp instance/target_value.cpp instance/target_value.hpp global_executor.cpp global_executor.hpp compiled_source.cpp
        source_compiler.cpp
        source_compiler.hpp)

add_library(exec ${EXEC_SOURCES})
target_include_directories(exec PRIVATE ${CMAKESL_SOURCES_DIR} ${CMAKESL_FACADE_DIR})
target_link_libraries(exec PRIVATE sema)


if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX exec FILES ${EXEC_SOURCES})
endif()

