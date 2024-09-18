#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mosquitto.h>

// 回调函数: 连接建立时被调用
static void on_connect(struct mosquitto *mosq, void *obj, int result)
{

    if (result != 0)
    {
        printf("Connected to Emqx %s\n",mosquitto_strerror(result));
    }
}

// 回调函数: 当接收到消息时被调用
static void on_subscribe(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{

    printf("Received message: %s\n", (char *)message->payload);
}

void main()
{
    // 创建 Mosquitto 客户端实例
    struct mosquitto *client = mosquitto_new("C_Emqx", true, NULL);
    if (!client)
    {

        fprintf(stderr, "Error creating client!\n");
    }

    // 设置回调函数
    mosquitto_connect_callback_set(client, on_connect);
    mosquitto_message_callback_set(client, on_subscribe);

    // 连接到 MQTT 服务器
    if (mosquitto_connect(client, "192.168.0.221", 1883, 60))
    {
        fprintf(stderr, "Error connecting to Emqx!\n");
        mosquitto_destroy(client);
    }

    // 订阅主题
    if (mosquitto_subscribe(client, NULL, "mdsd", 0))
    {
        fprintf(stderr, "Error subscribing to topic!\n");
        mosquitto_disconnect(client);
        mosquitto_destroy(client);
    }

    // 主循环
    while (1)
    {
        mosquitto_loop(client, -1, 1);
    }

    // 清理
    mosquitto_disconnect(client);
    mosquitto_destroy(client);
}