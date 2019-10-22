set(LEXER_SOURCES
    lexer.cpp
    lexer.hpp
    source_location_manipulator.cpp
    source_location_manipulator.hpp
    token.cpp
    token.hpp
    token_type.cpp
    token_type.hpp
)

add_library(lexer "${LEXER_SOURCES}")
target_include_directories(lexer
    PRIVATE
        ${CMAKESL_SOURCES_DIR}
)

target_link_libraries(lexer
    PUBLIC
        common
)

target_compile_options(lexer
    PRIVATE
        ${CMAKESL_ADDITIONAL_COMPILER_FLAGS}
)

