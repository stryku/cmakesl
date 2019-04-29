set(ERRORS_SOURCES
    error.hpp
    errors_observer.hpp
    errors_observer.cpp
)

cmsl_add_library(errors "${ERRORS_SOURCES}")
target_include_directories(errors
    PRIVATE
        ${CMAKESL_SOURCES_DIR}
)
