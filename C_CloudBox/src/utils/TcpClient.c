/**
 * Created by WangYunwei [2024-09-13]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#include "../include/Utils.h"

#define HEARTBEAT_INTERVAL 5 // 心跳间隔（秒）

void *TCP_Create_Client()
{
    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("创建 TCP_Socket 失败!");
    }

    // 填充服务器地址结构
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3490);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        fprintf(stderr, "连接服务器失败!\n");
        exit(EXIT_FAILURE);
    }

    // time_t current_time;
    // time(&current_time);

    time_t last_heartbeat = 0;
    while (1)
    {
        // 检查是否需要发送心跳
        time_t current_time = time(NULL);
        if (current_time - last_heartbeat >= HEARTBEAT_INTERVAL)
        {
            send(sockfd, "HEARTBEAT", 9, 0); // 发送心跳包
            printf("Sent heartbeat to server_%s.\n", ctime(&current_time));
            last_heartbeat = current_time;
        }
    }

    // 发送数据
    char sen_buffer[18] = "Hello from client";
    send(sockfd, sen_buffer, strlen(sen_buffer), 0);

    // 接收数据
    char rec_buffer[18] = {0};
    memset(rec_buffer, '\0', 18); // 用来在接收数据之前清空 buffer
    recv(sockfd, rec_buffer, 17, 0);
    printf("Received from server: %s", rec_buffer);

    // 关闭套接字
    close(sockfd);
}