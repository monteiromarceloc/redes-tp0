#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TAM 2
#define BUFSZ 128

void _logexit(const char *str) {
	perror(str);
	exit(EXIT_FAILURE);
}

void _usage(char* name) {
    printf("usage: %s <server IP> <server port>\n", name);
    printf("example: %s: 127.0.0.1 51511\n", name);
    exit(EXIT_FAILURE);
}

void _usage2(char *name) {
    printf("usage: %s <v4|v6> <server port>\n", name); // TODO: remove v4|v6 and fid a way to recognize IP family
    printf("example: %s: v4 51511\n", name);
    exit(EXIT_FAILURE);
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
        if(!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr, INET6_ADDRSTRLEN + 1)) _logexit("ntop");
        port = ntohs(addr4->sin_port); // network to host short

    } else if (addr->sa_family == AF_INET6) {
        version = 6;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        if(!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr, INET6_ADDRSTRLEN + 1)) _logexit("ntop");
        port = ntohs(addr6->sin6_port); // network to host short

    } else _logexit("addrtostr unkown protocol family");

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

int initSocketServer(int argc, char **argv){
    if (argc < 3) _usage2(argv[0]);

    struct sockaddr_storage storage; // Handle socket ipv4 or ipv6
    if(0 != server_sockaddr_init(argv[1], argv[2], &storage)) _usage2(argv[0]);

    int s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s == -1) _logexit("socket");

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
        _logexit("setsockopt");

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(0 != bind(s, addr, sizeof(storage))) _logexit("bind");
    if(0 != listen(s, 10)) _logexit("listen");
    
    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connection\n", addrstr);
    return s;
}

int acceptConnection(int s) {
    struct sockaddr_storage *cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
    socklen_t caddrlen = sizeof(cstorage);

    int csock = accept(s, caddr, &caddrlen);
    if(csock == -1) _logexit("accept");

    char caddrstr[BUFSZ];
    addrtostr(caddr, caddrstr, BUFSZ);
    printf("[log] connection from %s\n", caddrstr);
    return csock;
}

int connectToServer(int argc, char **argv) {
    if (argc < 3) _usage(argv[0]);
    struct sockaddr_storage storage; // Handle socket ipv4 or ipv6
    if(0 != addrparse(argv[1], argv[2], &storage)) _usage(argv[0]);

    int s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s == -1) _logexit("socket");

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(0 != connect(s, addr, sizeof(storage))) _logexit("connect"); // storage has size of addr content

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("connected to %s\n", addrstr);
    return s;
}