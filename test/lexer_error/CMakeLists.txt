set(CMAKESL_LEXER_ERROR_TEST_SOURCES
    lexer_error_test.cpp
    main.cpp
)

set(INCLUDE_DIRS
    ${CMAKESL_SOURCES_DIR}
    ${CMAKESL_TESTS_DIR}
)

set(LIBS
    lexer
    errors_observer_mock
)

cmsl_add_test(lexer_error
    "${CMAKESL_LEXER_ERROR_TEST_SOURCES}"
    "${INCLUDE_DIRS}"
    "${LIBS}"
)
