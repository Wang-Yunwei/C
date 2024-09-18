/**
 * Created by WangYunwei [2024-09-13]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 256
#define PORT 3490

int main(void) {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // 创建套接字
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 填充服务器地址结构
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 连接到服务器
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // 发送数据给服务器
    printf("Enter message to send to the server:\n");
    fgets(buffer, BUFFER_SIZE, stdin);
    send(sock, buffer, strlen(buffer), 0);

    // 接收来自服务器的数据
    memset(buffer, '\0', BUFFER_SIZE);
    recv(sock, buffer, BUFFER_SIZE - 1, 0);
    printf("Received from server: %s", buffer);

    close(sock);
    return 0;
}


// #include <sys/socket.h>
void *TCP_Create_Socket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd != 0)
    {
        perror("创建 TCP_Socket 失败!");
    }

}