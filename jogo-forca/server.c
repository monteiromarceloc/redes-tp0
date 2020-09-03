#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#define BUFSZ 1024
#define WORD "abcd" // TODO: handle casesensitiveness

void usage(char *name) {
    printf("usage: %s <v4|v6> <server port>\n", name); // TODO: remove v4|v6 and fid a way to recognize IP family
    printf("example: %s: v4 51511\n", name);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc < 3) usage(argv[0]);

    struct sockaddr_storage storage; // Handle socket ipv4 or ipv6
    if(0 != server_sockaddr_init(argv[1], argv[2], &storage)) usage(argv[0]);

    int s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s == -1) logexit("socket");

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
        logexit("setsockopt");

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(0 != bind(s, addr, sizeof(storage))) logexit("bind");
    if(0 != listen(s, 10)) logexit("listen");
    
    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connection\n", addrstr);

    while(1){
        struct sockaddr_storage *cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if(csock == -1) logexit("accept");

        char caddrstr[BUFSZ];
        addrtostr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);
// ------------------------------------------------------------------
        unsigned int tam = strlen(WORD);
        send1(csock, tam);
        recvBytes(csock);

        close(csock);
    }
    exit(EXIT_SUCCESS);
    return 0;
}