#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <inttypes.h>
#include <arpa/inet.h>

#define TAM 2

void logexit(const char *str) {
	perror(str);
	exit(EXIT_FAILURE);
}

void send1(int s, unsigned int tam) {
    unsigned char msg[TAM]; 
    msg[0] = 1;
    msg[1] = (unsigned char)tam;
    int count = send(s, msg, TAM, 0);
    if(count != TAM) logexit("send");
}

void send2(int s) {
    printf("palpite> ");
    unsigned char msg[TAM]; 
    msg[0] = 2;
    msg[1] = getchar();
    int count = send(s, msg, TAM, 0);
    if(count != TAM) logexit("send");
}

void send3(int s, int count, int* pos) {
    unsigned char msg[2+count]; 
    msg[0] = 3;
    msg[1] = count;
    for(int i=2; i<2+count; i++)
        msg[i]=pos[i];
    int count2 = send(s, msg, TAM, 0);
    if(count2 != TAM) logexit("send");
}

unsigned char recvByte(int s){
    unsigned char buf[TAM]; 
    memset(buf, 0, TAM);
    size_t count = recv(s, buf, TAM, 0);
    if(TAM != (int)count) logexit("recvByte");
    // printf("recieved %d bytes: %u %u\n", (int)count, buf[0], buf[1]);
    return buf[1];
}

void recvAnswer(int s, int max){
    unsigned char buf[max]; 
    memset(buf, 0, max);
    size_t count = recv(s, buf, max, 0);
    printf("count: %d tipo: %u ocorrencias: %u\n", (int)count, buf[0], buf[1]);
}

int charFind(char c, char* s, int len, int* pos){
    int count = 0;
    for(int i=0; i<len; i++) {
        if(s[i] == c) {
            pos[count]=i;
            count++;
        }
    }
    return count;
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