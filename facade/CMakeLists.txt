#[[
set(FACADE_SOURCES
        cmake_facade.hpp
)

add_library(facade ${FACADE_SOURCES})
target_include_directories(facade PRIVATE
        ${CMAKESL_SOURCES_DIR}
        ${CMAKESL_SOURCES_DIR}/.. # path to CMake/Source
)

target_link_libraries(facade CMakeLib)

set(CMAKESL_FACADE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE . PREFIX ast FILES ${FACADE_SOURCES})
endif()
]]