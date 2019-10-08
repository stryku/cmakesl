cmake_minimum_required(VERSION 3.5)

project(CMakeSL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKESL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKESL_DOC_DIR ${CMAKESL_DIR}/doc)
set(CMAKESL_SCRIPTS_DIR ${CMAKESL_DIR}/scripts)
set(CMAKESL_SOURCES_DIR ${CMAKESL_DIR}/source)

include_directories(facade)
add_subdirectory(source)

option(CMAKESL_WITH_TOOLS "When ON, tools will be built" OFF)
if (CMAKESL_WITH_TOOLS)
    add_subdirectory(tools)
endif ()

option(CMAKESL_WITH_EXAMPLES "When ON, examples will be built" OFF)
if (CMAKESL_WITH_EXAMPLES)
    if(NOT CMAKESL_WITH_TOOLS)
        message(FATAL_ERROR
                Examples can not be built without tools support.
                Enable CMAKESL_WITH_TOOLS option and try again)
    endif()

    add_subdirectory(examples)
endif ()

option(CMAKESL_WITH_TESTS "When ON, test will be built" OFF)
if (CMAKESL_WITH_TESTS)
    enable_testing()
    include(CTest)
    add_subdirectory(external/googletest EXCLUDE_FROM_ALL)
    add_subdirectory(test)
endif ()
