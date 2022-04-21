project(http-server)
cmake_minimum_required(VERSION 3.16)

add_compile_options(-g)

add_subdirectory(lib)

add_executable(main_sub_reactors_epoll_server main_sub_reactors_epoll_server.cpp)
target_link_libraries(main_sub_reactors_epoll_server mylib)

add_executable(http_server_test http_server_test.cpp)
target_link_libraries(http_server_test mylib)

add_executable(http_client http_client.cpp)