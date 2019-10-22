set(ERRORS_SOURCES
    error.hpp
    errors_observer.hpp
    errors_observer.cpp
)

add_library(errors "${ERRORS_SOURCES}")

target_include_directories(errors
    PRIVATE
        ${CMAKESL_SOURCES_DIR}
)

target_compile_options(errors
    PRIVATE
        ${CMAKESL_ADDITIONAL_COMPILER_FLAGS}
)
