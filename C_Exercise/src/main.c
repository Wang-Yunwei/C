/**
 * Created by WangYunwei [2024-09-11]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>  
#include <ifaddrs.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>

#include "include/Utils.h"

void startMqtt();
void startTcp();

void test1(char *str)
{
    printf("%s %ld\n", str, strlen(str));
}
void main()
{
    printf("========= Main Function Start! =========\n");

    test1("Hello World!");


    
    pthread_t thread_id_udp;
    pthread_create(&thread_id_udp, NULL, UDP_Create_Socket, NULL);
    pthread_join(thread_id_udp, NULL);
    

    printf("========= Main Function End! =========\n");
}

void startTcp()
{
    pthread_t thread_id_tcp_server;
    pthread_create(&thread_id_tcp_server, NULL, TCP_Create_Server, NULL);

    sleep(3);

    pthread_t thread_id_tcp_client;
    pthread_create(&thread_id_tcp_client, NULL, TCP_Create_Client, NULL);

    // 等待子线程结束
    // pthread_join(thread_id_tcp_client, NULL);
    pthread_join(thread_id_tcp_server, NULL);
}

void startMqtt()
{
    /* --------- Start _ MQTT ---------*/

    pthread_t thread_id_mqtt;
    if (pthread_create(&thread_id_mqtt, NULL, MQTT_CreateClient, NULL))
    {
        fprintf(stderr, "Error creating thread\n");
        return;
    }
    // 主线程休眠一会
    sleep(3);
    char *str = "Hello This is a test message!";
    if (MQTT_v5_publish(str))
    {
        printf("发布消息失败!\n");
    }

    /* --------- Start _ UDP ---------*/

    // 等待子线程结束
    pthread_join(thread_id_mqtt, NULL);
}