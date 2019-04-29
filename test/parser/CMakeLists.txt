set(CMAKESL_TEST_SOURCES 
    main.cpp
    parser_test.cpp
)

set(INCLUDE_DIRS
    ${CMAKESL_SOURCES_DIR}
    ${CMAKESL_TESTS_DIR}
    ${CMAKESL_DIR}
)

set(LIBS
    lexer
    ast
    errors
    tests_common
)

cmsl_add_test(parser
    "${CMAKESL_TEST_SOURCES}"
    "${INCLUDE_DIRS}"
    "${LIBS}"
)
