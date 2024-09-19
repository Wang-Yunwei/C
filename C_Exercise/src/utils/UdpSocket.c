/**
 * Created by WangYunwei [2024-09-13]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int sockfd;

// 关闭套接字
void UDP_Socket_Close()
{
    close(sockfd);
}

// 发送数据
void UDP_Socket_Send(char *message, const char *server_ip, int server_port)
{

    // 设置服务器地址结构
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    // 将IPv4和IPv6地址从文本转换成二进制形式
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    int send_bytes = sendto(sockfd, message, strlen(message) + 1, MSG_DONTWAIT,
                            (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (send_bytes < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            printf("Send would block\n");
        }
        else
        {
            perror("sendto failed");
        }
        return;
    }
}

void *UDP_Create_Socket()
{
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd != 0)
    {
        perror("创建 UDP_Socket 失败!");
        exit(EXIT_FAILURE);
    }

    // 循环接收数据
    char buffer[1024] = {0};
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    while (1)
    {
        // 设置非阻塞模式
        int flags = fcntl(sockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

        int recv_bytes = recvfrom(sockfd, buffer, 1024, MSG_DONTWAIT, (struct sockaddr *)&from, &fromlen);
        if (recv_bytes < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                printf("接收将阻塞!\n");
            }
            else
            {
                perror("接收数据失败!\n");
            }
            return;
        }
    }
    printf("Received: %s\n", buffer);
}