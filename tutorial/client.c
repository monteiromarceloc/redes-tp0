#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 1024

void usage(char* name) {
    printf("usage: %s <server IP> <server port>\n", name);
    printf("example: %s: 127.0.0.1 51511\n", name);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc < 3) usage(argv[0]);

    struct sockaddr_storage storage; // Handle socket ipv4 or ipv6
    if(0 != addrparse(argv[1], argv[2], &storage)) usage(argv[0]);

    int s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s == -1) logexit("socket");

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(0 != connect(s, addr, sizeof(storage))) logexit("connect"); // storage has size of addr content

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("connected to %s\n", addrstr);

    char buf[BUFSZ];
    memset(buf, 0, BUFSZ); // Inicializar buffer com 0
    printf("msg> ");
    fgets(buf, BUFSZ-1, stdin);
    int count = send(s, buf, strlen(buf)+1, 0); // (socket, dado, tamanho, op)
    if(count != strlen(buf)+1) logexit("send"); // count = numero de bytes efetivamente transmitido

    // recv
    memset(buf, 0, BUFSZ);
    unsigned total = 0;
    while(1){
        count = recv(s, buf + total, BUFSZ-total, 0);
        if(count == 0) break;
        total += count;
    }
    close(s);
    printf("recieved %u bytes:\n%s\n", total, buf);
    exit(EXIT_SUCCESS);
    return 0;
}