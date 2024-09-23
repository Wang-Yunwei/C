/**
 * Created by WangYunwei [2024-09-13]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../include/Utils.h"

#define SERVER_IP "192.168.0.221"
#define SERVER_PORT 20242
#define BUFFER_SIZE 1400

int sockudp;

// 关闭套接字
void UDP_Socket_Close()
{
    close(sockudp);
}

// 发送数据
void UDP_Socket_Send(char *message)
{
    // 设置服务器地址结构
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr)); // 将服务器地址结构体清零
    server_addr.sin_family = AF_INET;             // 设置地址族为IPv4
    server_addr.sin_port = htons(SERVER_PORT);    // 设置服务器端口, 并将其转换为网络字节序
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    char buffer[BUFFER_SIZE];
    strncpy(buffer, message, BUFFER_SIZE);

    // 发送数据包
    if (sendto(sockudp, message, strlen(message), MSG_DONTWAIT, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Send failed");
    }
}

void *UDP_Create_Socket()
{
    // 创建套接字
    sockudp = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockudp < 0)
    {
        perror("创建 UDP_Socket 失败!");
        exit(EXIT_FAILURE);
    }

    // 获取本机IP地址
    getAddr(sockudp);

    // 绑定端口
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(20243);
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockudp, (struct sockaddr *)&local, sizeof(local)))
    {
        perror("绑定失败");
        close(sockudp);
        exit(EXIT_FAILURE);
    }

    // 设置非阻塞模式
    int flags = fcntl(sockudp, F_GETFL, 0);
    fcntl(sockudp, F_SETFL, flags | O_NONBLOCK);

    // 循环接收数据
    char buffer[1024] = {0};
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    while (1)
    {
        ssize_t received = recvfrom(sockudp, buffer, 1024, MSG_DONTWAIT, (struct sockaddr *)&from, &fromlen);
        if (received > 0)
        {
            buffer[received] = '\0'; // 确保字符串以null终止
            printf("Received: %s\n", buffer);
        }
        else if (received == -1 && errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // 没有数据可读，这里可以做一些其他事情或者等待
            usleep(100000); // 休眠100毫秒
        }
        else
        {
            perror("接收失败");
            break;
        }
    }
    close(sockudp); // 关闭套接字
    return NULL;
}