#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/types.h>  
#include <ifaddrs.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
  
int main() {  
    struct ifaddrs *ifaddr, *ifa;  
    int family, s;  
    char host[128];  
  
    if (getifaddrs(&ifaddr) == -1) {  
        perror("getifaddrs");  
        exit(EXIT_FAILURE);  
    }  
  
    /* Walk through linked list, maintaining head pointer so we can free list later */  
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {  
        if (!ifa->ifa_addr)  
            continue;  
  
        family = ifa->ifa_addr->sa_family;  
  
        /* Display interface name and family (including IPv4 and IPv6) */  
        printf("%s  address family: %d\n", ifa->ifa_name, family);  
  
        if (family == AF_INET || family == AF_INET6) {  
            s = getnameinfo(ifa->ifa_addr,  
                            (family == AF_INET) ? sizeof(struct sockaddr_in) :  
                            sizeof(struct sockaddr_in6),  
                            host, 128,  
                            NULL, 0, 0);  
            if (s != 0) {  
                printf("getnameinfo() failed: %s\n", gai_strerror(s));  
                exit(EXIT_FAILURE);  
            }  
  
            printf("\taddress: %s\n", host);  
        }  
    }  
  
    freeifaddrs(ifaddr);  
    return 0;  
}