/**
 * Created by WangYunwei [2024-09-18]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAX_CONNECTIONS 5
#define BUFFER_SIZE 256
#define PORT 3490

void *TCP_Create_Server()
{
    int conn_sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    fd_set master_fds, read_fds;
    int max_fd = 0;

    // 创建监听套接字
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 设置地址重用选项
    int optval = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // 绑定地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // 监听连接
    if (listen(socketfd, MAX_CONNECTIONS) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // 初始化文件描述符集
    FD_ZERO(&master_fds);
    FD_SET(socketfd, &master_fds);
    max_fd = socketfd;

    while (1)
    {
        read_fds = master_fds;
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            continue;
        }

        // 检查是否有新连接
        if (FD_ISSET(socketfd, &read_fds))
        {
            socklen_t addr_len = sizeof(server_addr);
            if ((conn_sock = accept(socketfd, (struct sockaddr *)&server_addr, &addr_len)) == -1)
            {
                perror("accept");
                continue;
            }
            FD_SET(conn_sock, &master_fds); // 添加到监视集合
            if (conn_sock > max_fd)
                max_fd = conn_sock;
            printf("New client connected: %d\n", conn_sock);
        }

        // 检查已连接的客户端是否可读
        for (int i = 0; i <= max_fd; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                memset(buffer, '\0', BUFFER_SIZE);
                int bytes_received = recv(i, buffer, BUFFER_SIZE - 1, 0);
                if (bytes_received > 0)
                {
                    printf("Received from client %d: %s\n", i, buffer);
                    send(i, buffer, bytes_received, 0);
                }
                else if (bytes_received == 0)
                {
                    printf("Client disconnected: %d\n", i);
                    close(i);
                    FD_CLR(i, &master_fds);
                }
                else
                {
                    perror("recv");
                }
            }
        }
    }

}