set(CMAKESL_PARSER_ERROR_TEST_SOURCES 
    main.cpp
    parser_error_test.cpp
)

set(TEST_NAME parser_error_test)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CMAKESL_PARSER_ERROR_TEST_SOURCES})
endif()

add_executable(${TEST_NAME} ${CMAKESL_PARSER_ERROR_TEST_SOURCES})
target_link_libraries(${TEST_NAME} gmock_main ast lexer errors_observer_mock tests_common)
target_include_directories(${TEST_NAME} PRIVATE 
    ${CMAKESL_SOURCES_DIR}
    ${CMAKESL_TESTS_DIR}
    ${CMAKESL_DIR}
)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
