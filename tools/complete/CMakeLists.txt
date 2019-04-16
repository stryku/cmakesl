set(COMPLETE_SOURCES
        cmsl_complete.cpp
        cmsl_complete.hpp
        cmsl_parsed_source.hpp
        completer.cpp
        completer.hpp
        completion_context_finder.cpp
        completion_context_finder.hpp
        completion_contextes_visitor.cpp completion_contextes_visitor.hpp type_names_collector.cpp type_names_collector.hpp)

add_library(cmsl_complete ${COMPLETE_SOURCES})
target_include_directories(cmsl_complete PRIVATE ${CMAKESL_SOURCES_DIR} ${CMAKESL_FACADE_DIR})
target_link_libraries(cmsl_complete PRIVATE sema)


if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX cmsl_complete FILES ${COMPLETE_SOURCES})
endif()

