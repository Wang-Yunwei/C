#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <mosquitto.h>

#include "../include/Utils.h"

#define MQTT_HOST "192.168.0.221"
#define MQTT_PORT 1883
#define MQTT_KEEPALIVE 60
#define MQTT_USER "mdsd"
#define MQTT_PASSWORD "mdsd"
#define MQTT_CLIENTID "C_CloudBox_1.0.0"
#define MQTT_SUB_TOPIC "C_CloudBox/Subscribe/M350"
#define MQTT_PUB_TOPIC "C_CloudBox/Publish/M350"

struct mosquitto *client;

// 释放资源
void on_clear()
{
    int reason_code;
    mosquitto_disconnect_v5(client, reason_code, NULL);
    fprintf(stderr, "Disconnected from Emqx Reason Code: %d\n", reason_code);
    mosquitto_destroy(client);
}

// 回调函数: 发布消息
int MQTT_v5_publish(char *message)
{
    int res = mosquitto_publish_v5(client, NULL, MQTT_SUB_TOPIC, strlen(message), message, 0, false, NULL);
    if (res == MOSQ_ERR_SUCCESS)
    {
        printf(">>> on_v5_publish: %s\n", message);
    }
    return res;
}

// 回调函数: 订阅消息
void on_v5_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message, const mosquitto_property *props)
{
    printf("<<< on_v5_message: %s\n", (char *)message->payload);
}

// 回调函数: 连接成功
void on_v5_connect(struct mosquitto *mosq, void *obj, int valA, int valB, const mosquitto_property *props)
{
    printf(">>> 连接MQTT 服务器成功!\n");
}

// 创建客户端
void *MQTT_CreateClient()
{
    // 创建 Mosquitto 客户端实例
    client = mosquitto_new(MQTT_CLIENTID, true, NULL);
    if (!client)
    {
        fprintf(stderr, "创建MQTT客户端失败!\n");
    }

    // 设置 MQTT 版本
    mosquitto_int_option(client, MOSQ_OPT_PROTOCOL_VERSION, MQTT_PROTOCOL_V5);

    // 设置账号和密码
    if (mosquitto_username_pw_set(client, MQTT_USER, MQTT_PASSWORD))
    {
        fprintf(stderr, "设置账号和密码失败!\n");
    }

    // 设置回调函数
    mosquitto_connect_v5_callback_set(client, on_v5_connect);
    mosquitto_message_v5_callback_set(client, on_v5_message);

    // 连接到 MQTT 服务器
    int con, count = 0;
    con = mosquitto_connect_bind_v5(client, MQTT_HOST, MQTT_PORT, MQTT_KEEPALIVE, NULL, NULL);
    while (con != 0)
    {
        sleep(5);
        fprintf(stderr, "连接到MQTT服务器失败, 尝试重新连接!\n");
        con = mosquitto_connect_bind_v5(client, MQTT_HOST, MQTT_PORT, MQTT_KEEPALIVE, NULL, NULL);
        if (count >= 3)
        {
            fprintf(stderr, "尝试重新连接次数超过三次, 则不再尝试!\n");
            mosquitto_destroy(client);
            return NULL;
        }
        count++;
    }

    // 订阅主题
    int options;
    if (mosquitto_subscribe_v5(client, NULL, MQTT_SUB_TOPIC, 0, options, NULL))
    {
        fprintf(stderr, "订阅主题失败!\n");
        on_clear();
    }

    // 循环等待消息
    while (1)
    {
        mosquitto_loop(client, -1, 1);
    }
}
