#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
# define    MAXLINE     4096
# define    SERV_PORT   43211

int main(int argc, char const *argv[])
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);
    int connect_rt = connect(socket_fd, (struct sockaddr *) &server_addr, server_len);
    if (connect_rt < 0) {
        printf("connect failed\n");
    }

    char str[] = "GET / HTTP/1.1\r\n\r\n\r\n";
    char read_buf[1000];
    for (int i = 0; i < 2; i++) {
        int n = write(socket_fd, str, std::strlen(str));
        printf("i=%d, write %d bytes\n", i, n);
        // sleep(5);
        int m = read(socket_fd, read_buf, 1000);
        // if (m > 0) {
        //     read_buf[m] = 0;
        //     printf("read:%s|\n", read_buf);
        // } else {
        //     printf("read %d bytes\n", m);
        // }
        // printf("read %d bytes\n", m);        
    }
    close(socket_fd);
    return 0;
}
