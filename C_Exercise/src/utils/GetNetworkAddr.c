/**
 * Created by WangYunwei [2024-09-20]
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

void getAddr(int sockudp)
{
    struct ifreq ifr;
    char ip[INET_ADDRSTRLEN];

    // 获取网络接口列表
    struct ifconf ifc;
    char buf[1024];
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;

    if (ioctl(sockudp, SIOCGIFCONF, &ifc) < 0)
    {
        perror("ioctl SIOCGIFCONF");
        close(sockudp);
        exit(EXIT_FAILURE);
    }

    // 遍历网络接口列表
    struct ifreq *it = ifc.ifc_req;
    const struct ifreq *const end = it + (ifc.ifc_len / sizeof(struct ifreq));
    for (; it != end; ++it)
    {
        strcpy(ifr.ifr_name, it->ifr_name);
        if (ioctl(sockudp, SIOCGIFADDR, &ifr) < 0)
        {
            perror("ioctl SIOCGIFADDR");
            continue;
        }

        struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
        inet_ntop(AF_INET, &addr->sin_addr, ip, INET_ADDRSTRLEN);

        // 打印接口名称和IP地址
        printf("%-8s %s\n", it->ifr_name, ip);
    }
}