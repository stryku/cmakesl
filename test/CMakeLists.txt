set(CMAKESL_TEST_SOURCES main.cpp)

add_subdirectory(lexer)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CMAKESL_TEST_SOURCES})

add_executable(cmakesl_test ${CMAKESL_TEST_SOURCES})
target_link_libraries(cmakesl_test gmock_main)
add_test(NAME cmakesl_test COMMAND cmakesl_test)
