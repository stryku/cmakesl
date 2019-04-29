set(CMAKESL_TEST_COMMON_SOURCES
    tokens.cpp
    tokens.hpp
)

cmsl_add_library(tests_common "${CMAKESL_TEST_COMMON_SOURCES}")
target_include_directories(tests_common
    PRIVATE
        ${CMAKESL_SOURCES_DIR}
        ${CMAKESL_DIR}
)
