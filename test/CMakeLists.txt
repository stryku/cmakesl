add_executable(example main.cpp)
target_link_libraries(example gmock_main)
add_test(NAME example_test COMMAND example)
