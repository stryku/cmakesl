set(COMMON_SOURCES
    string.hpp
)

add_library(common ${COMMON_SOURCES})
set_target_properties(common PROPERTIES LINKER_LANGUAGE CXX)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX common FILES ${COMMON_SOURCES})
