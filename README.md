# 基于主从 reactor 多线程模型和 epoll 的 http server
- 实现了事件驱动模型
- 实现了tcp server：reactor主线程通过 epoll 监听连接事件，为新连接分配子线程，子线程监听并处理读写事件
- 基于上述 tcp server 实现了 http server

http server 使用示例： [http_server_test.cpp](https://github.com/Jessense/http-server/blob/main/http_server_test.cpp)

tcp server 使用示例：[main_sub_reactors_epoll_server.cpp](https://github.com/Jessense/http-server/blob/main/main_sub_reactors_epoll_server.cpp)