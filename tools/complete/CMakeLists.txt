set(COMPLETE_SOURCES
        cmsl_complete.cpp
        cmsl_complete.hpp
        cmsl_parsed_source.hpp
)

add_library(cmsl_complete ${COMPLETE_SOURCES})
target_include_directories(cmsl_complete PRIVATE ${CMAKESL_SOURCES_DIR} ${CMAKESL_FACADE_DIR})
target_link_libraries(cmsl_complete PRIVATE sema)


if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX cmsl_complete FILES ${COMPLETE_SOURCES})
endif()

