project(playground)

include_directories(${CMAKESL_SOURCES_DIR})

add_executable(playground main.cpp)
target_link_libraries(playground exec lexer ast errors)

