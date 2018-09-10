cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)

project(CMakeSL)

if(CMAKESL_WITH_TESTS)
    # Download and unpack googletest at configure time
    configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
    if(result)
      message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
    if(result)
      message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    # Prevent overriding the parent project's compiler/linker
    # settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                     ${CMAKE_BINARY_DIR}/googletest-build
                     EXCLUDE_FROM_ALL)

    # The gtest/gtest_main targets carry header search path
    # dependencies automatically when using CMake 2.8.11 or
    # later. Otherwise we have to add them here ourselves.
    if (CMAKE_VERSION VERSION_LESS 2.8.11)
      include_directories(BEFORE SYSTEM
        "${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")
    else()
      target_include_directories(gmock_main SYSTEM BEFORE INTERFACE
        "${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")
    endif()
endif()

find_package(Boost REQUIRED)
if (NOT Boost_FOUND)
    message (ERROR "Boost not found!")
endif()
include_directories (${Boost_INCLUDE_DIRS})

set(CMAKESL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKESL_SOURCES_DIR ${CMAKESL_DIR}/source)

include_directories(facade)
add_subdirectory(facade)
add_subdirectory(source)

if(CMAKESL_WITH_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

add_subdirectory(playground)

