project(playground)

add_executable(playground main.cpp)
target_link_libraries(playground lexer exec ast)

