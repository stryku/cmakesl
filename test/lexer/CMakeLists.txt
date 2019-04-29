set(CMAKESL_TEST_SOURCES
    lexer_test.cpp
    main.cpp
)

set(INCLUDE_DIRS
    ${CMAKESL_SOURCES_DIR}
)

set(LIBS
    lexer
    errors
)

cmsl_add_test(lexer
    "${CMAKESL_TEST_SOURCES}"
    "${INCLUDE_DIRS}"
    "${LIBS}"
)
