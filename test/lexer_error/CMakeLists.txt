set(CMAKESL_LEXER_ERROR_TEST_SOURCES 
    main.cpp
    lexer_error_test.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CMAKESL_LEXER_ERROR_TEST_SOURCES})

add_executable(cmakesl_lexer_error_test ${CMAKESL_LEXER_ERROR_TEST_SOURCES})
target_link_libraries(cmakesl_lexer_error_test gmock_main common lexer errors_observer_mock)
target_include_directories(cmakesl_lexer_error_test PRIVATE 
    ${CMAKESL_SOURCES_DIR}
    ${CMAKESL_TESTS_DIR}
)
add_test(NAME cmakesl_lexer_error_test COMMAND cmakesl_lexer_error_test)
