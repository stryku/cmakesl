cmake_minimum_required(VERSION 3.5)


project(CMakeSL)

add_definitions(--std=c++17)

set(CMAKESL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKESL_SOURCES_DIR ${CMAKESL_DIR}/source)

include_directories(facade)
add_subdirectory(source)
add_subdirectory(tools)
add_subdirectory(examples)

if(CMAKESL_WITH_TESTS)
    enable_testing()
    # Todo: move to external/CMakeLists.txt
    add_subdirectory( external/googletest)
    add_subdirectory(test)
endif()

add_subdirectory(playground)

