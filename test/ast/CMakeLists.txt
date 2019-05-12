set(AST_TEST_SOURCES
    main.cpp
    parser_test.cpp
)

set(INCLUDE_DIRS
    ${CMAKESL_SOURCES_DIR}
    ${CMAKESL_DIR}
)

set(LIBS
    ast
    lexer
    errors
    tests_common
)

cmsl_add_test(ast
    "${AST_TEST_SOURCES}"
    "${INCLUDE_DIRS}"
    "${LIBS}"
)

set(AST_ERROR_TEST_SOURCES
    main.cpp
    parser_test.cpp
)

set(LIBS
    ast
    lexer
    errors_observer_mock
    tests_common
)

cmsl_add_test(ast_error
    "${AST_ERROR_TEST_SOURCES}"
    "${INCLUDE_DIRS}"
    "${LIBS}"
)
