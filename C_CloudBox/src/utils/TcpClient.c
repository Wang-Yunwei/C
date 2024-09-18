/**
 * Created by WangYunwei [2024-09-13]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void *TCP_Create_Client()
{
    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd != 0)
    {
        perror("创建 TCP_Socket 失败!");
    }

    // 填充服务器地址结构
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3490);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 将 IPv4 地址从文本转换成二进制形式
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr))
    {
        fprintf(stderr, "inet_pton() 失败!\n");
    }

    // 连接服务端
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        fprintf(stderr, "连接服务器失败!\n");
        exit(EXIT_FAILURE);
    }

    // 发送数据
    char sen_buffer[18] = "Hello from client";
    send(sockfd, sen_buffer, strlen(sen_buffer), 0);

    // 接收数据
    char rec_buffer[18] = {0};
    recv(sockfd, rec_buffer, 17, 0);
    printf("Received from server: %s", rec_buffer);

    // 关闭套接字
    close(sockfd);
}