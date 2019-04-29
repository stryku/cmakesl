cmake_minimum_required(VERSION 3.5)

project(CMakeSL)

include(cmake/cmsl_cmake_utils.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKESL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKESL_SOURCES_DIR ${CMAKESL_DIR}/source)

include_directories(facade)
add_subdirectory(source)
add_subdirectory(tools)
add_subdirectory(examples)

if(CMAKESL_WITH_TESTS)
    enable_testing()
    add_subdirectory(external/googletest)
    add_subdirectory(test)
endif()
