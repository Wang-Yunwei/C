/**
 * Created by WangYunwei [2024-09-11]
 */
#ifndef C_CLOUD_BOX_UTILS_H
#define C_CLOUD_BOX_UTILS_H

void *MQTT_CreateClient();
int MQTT_v5_publish(char *message);

void *TCP_Create_Server();
void *TCP_Create_Client();

void *UDP_Create_Socket();
void UDP_Socket_Send(char *message);

#endif