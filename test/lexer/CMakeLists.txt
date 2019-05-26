cmsl_add_test(
    NAME
        lexer
    SOURCES
        lexer_test.cpp
        main.cpp
    INCLUDE_DIRS
        ${CMAKESL_SOURCES_DIR}
    LIBRARIES
        lexer
        errors
)

