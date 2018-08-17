set(COMMON_SOURCES
    algorithm.hpp
    source_location.hpp
    enum_class_map.hpp
    string.hpp
)

add_library(common ${COMMON_SOURCES})
set_target_properties(common PROPERTIES LINKER_LANGUAGE CXX)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX common FILES ${COMMON_SOURCES})
endif()

target_include_directories(common PRIVATE ${CMAKESL_SOURCES_DIR})
