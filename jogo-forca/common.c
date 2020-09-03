#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <inttypes.h>
#include <arpa/inet.h>

#define BUFSZ 1024

void logexit(const char *str) {
	perror(str);
	exit(EXIT_FAILURE);
}

void sendMsg(int s, char* msg) {
    int count = send(s, msg, strlen(msg)+1, 0);
    if(count != strlen(msg)+1) logexit("send");
}

void serverRecv(int s){
    char buf[BUFSZ];
    memset(buf, 0, BUFSZ);
    size_t count = recv(s, buf, BUFSZ, 0);
    printf("recieved %d bytes: %s\n", (int)count, buf);
}

void recvMsg(int s){
    char buf[BUFSZ];
    memset(buf, 0, BUFSZ);
    unsigned total = 0;
    int count = 0;
    while(1){
        count = recv(s, buf + total, BUFSZ-total, 0);
        if(count == 0) break;
        total += count;
    }
    printf("recieved %u bytes:\n%s\n", total, buf);
}

void getAndSendMsg(int s) {
    char buf[BUFSZ];
    memset(buf, 0, BUFSZ); // Inicializar buffer com 0
    printf("msg> ");
    fgets(buf, BUFSZ-1, stdin);
    sendMsg(s, buf);
}







int addrparse(const char*addrstr, const char *portstr, struct sockaddr_storage *storage) {
    if(addrstr == NULL || portstr == NULL) return -1;
    
    uint16_t port = (uint16_t)atoi(portstr); // unsigned short
    if (port == 0) return -1;
    port = htons(port); // host to network short

    struct in_addr inaddr4; // 32-bit IP addr
    if(inet_pton(AF_INET, addrstr, &inaddr4)) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6; // 128-bit IPv6 addr
    if(inet_pton(AF_INET6, addrstr, &inaddr6)) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        // addr6->sin6_addr = inaddr6;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6)); // deve ser feito asim já que é um arranjo de bytes
        return 0;
    }

    return -1;
}

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize) {
    int version;
    char addrstr[INET6_ADDRSTRLEN+1] = "";
    uint16_t port;
    if(addr->sa_family == AF_INET) {
        version = 4;
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        if(!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr, INET6_ADDRSTRLEN + 1)) logexit("ntop");
        port = ntohs(addr4->sin_port); // network to host short

    } else if (addr->sa_family == AF_INET6) {
        version = 6;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        if(!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr, INET6_ADDRSTRLEN + 1)) logexit("ntop");
        port = ntohs(addr6->sin6_port); // network to host short

    } else logexit("addrtostr unkown protocol family");

    if(str) snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port);
}

int server_sockaddr_init(const char *proto, const char *portstr, struct sockaddr_storage *storage) {

    uint16_t port = (uint16_t)atoi(portstr); // unsigned short
    if (port == 0) return -1;
    port = htons(port); // host to network short

    memset(storage, 0, sizeof(*storage));
    if (0 == strcmp(proto, "v4")){
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_addr.s_addr = INADDR_ANY; // sin_addr is struct with int (s_addr) inside
        addr4->sin_port = port;
        return 0;

    } else if (0 == strcmp(proto, "v6")){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_addr = in6addr_any;
        addr6->sin6_port = port;
        return 0;
    }
    return -1;
}