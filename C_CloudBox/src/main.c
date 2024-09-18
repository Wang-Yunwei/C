/**
 * Created by WangYunwei [2024-09-11]
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "include/Utils.h"

void main()
{
    printf("========= Main Function Start! =========\n");
    
    /* --------- Start _ MQTT ---------*/

    pthread_t thread_id_mqtt;
    if (pthread_create(&thread_id_mqtt, NULL, MQTT_CreateClient, NULL))
    {
        fprintf(stderr, "Error creating thread\n");
        return;
    }
    // 主线程休眠一会等待子线程结束
    sleep(3);
    char *str = "Hello This is a test message!";
    if (MQTT_v5_publish(str))
    {
        printf("发布消息失败!\n");
    }

    /* --------- Start _ UDP ---------*/

    

    // 等待子线程结束
    pthread_join(thread_id_mqtt, NULL);



    printf("========= Main Function End! =========\n");
}