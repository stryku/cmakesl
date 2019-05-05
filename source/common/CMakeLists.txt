set(COMMON_SOURCES
    algorithm.hpp
    assert.hpp
    enum_class_utils.hpp
    int_alias.hpp
    overloaded.hpp
    source_location.hpp
    source_view.cpp
    source_view.hpp
    string.hpp
)

add_library(common "${COMMON_SOURCES}")
target_include_directories(common
    PRIVATE
        ${CMAKESL_SOURCES_DIR}
)
