set(CMAKESL_PARSER_ERROR_TEST_SOURCES 
    main.cpp
    parser_error_test.cpp
)

set(INCLUDE_DIRS
    ${CMAKESL_SOURCES_DIR}
    ${CMAKESL_TESTS_DIR}
    ${CMAKESL_DIR}
)

set(LIBS
    ast
    lexer
    errors_observer_mock
    tests_common
)

cmsl_add_test(parser_error
    "${CMAKESL_PARSER_ERROR_TEST_SOURCES}"
    "${INCLUDE_DIRS}"
    "${LIBS}"
)
