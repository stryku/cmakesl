set(COMMON_SOURCES
    algorithm.hpp
    assert.hpp
    source_location.hpp
    enum_class_utils.hpp
    string.hpp
        source_view.cpp source_view.hpp)

add_library(common ${COMMON_SOURCES})

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX common FILES ${COMMON_SOURCES})
endif()

target_include_directories(common PRIVATE ${CMAKESL_SOURCES_DIR})
