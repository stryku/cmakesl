set(CMAKESL_TEST_SOURCES 
    main.cpp
    parser_test.cpp
)


if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CMAKESL_TEST_SOURCES})
endif()


set(TEST_NAME parser_test)

add_executable(${TEST_NAME} ${CMAKESL_TEST_SOURCES})
target_link_libraries(${TEST_NAME} gmock_main lexer ast errors tests_common)
target_include_directories(${TEST_NAME} PRIVATE 
    ${CMAKESL_SOURCES_DIR}
    ${CMAKESL_TESTS_DIR}
    ${CMAKESL_DIR}
)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
