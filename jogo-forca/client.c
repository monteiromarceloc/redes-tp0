#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 128

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
// ------------------------------------------------------------------

    int tamanho = (int)recvByte(s);
    printf("tamanho: %d\n", tamanho);

    send2(s);
    recvAnswer(s, tamanho+2);
    getchar();
    send2(s);
    recvAnswer(s, tamanho+2);

    close(s);
    exit(EXIT_SUCCESS);
    return 0;
}