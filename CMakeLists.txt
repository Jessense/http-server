project(http-server)
cmake_minimum_required(VERSION 3.16)
add_subdirectory(lib)

add_executable(test_loop test_loop.cpp)
target_link_libraries(test_loop mylib)

add_executable(test_loop2 test_loop2.cpp)
target_link_libraries(test_loop2 mylib)

add_executable(test_reactor test_reactor.cpp)
target_link_libraries(test_reactor mylib)