project(playground)

include_directories(${CMAKESL_SOURCES_DIR})

add_executable(playground main.cpp)
target_link_libraries(playground cmsl_complete exec lexer ast errors)
target_include_directories(playground PRIVATE ${CMAKESL_DIR}/tools/complete)

