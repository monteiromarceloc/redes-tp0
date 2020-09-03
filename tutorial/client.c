#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#define BUFSZ 1024

void usage() {
    printf("usage: %s <server IP> <server port>");
    printf("example: %s: 127.0.0.1 51511");
    exit(EXIT_FAILURE);
}

void logexit(const char *str) {
	perror(str);
	exit(EXIT_FAILURE);
}

void main(int argc, char **argv) {
    if (argc < 3) usage();

    // Conexão ativa
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == -1) logexit("socket");
    if(0 != connect(s, addr, sizeof(addr))) logexit("connect");

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    print("connected to %s\n");

    char buf[BUFSZ];
    memset(buf, 0, BUFSZ); // Inicializar buffer com 0
    printf("msg> ");
    fgets(buf, BUFSZ-1, stdin);
    int count = send(s, buf, strlen(buf)+1, 0); // (socket, dado, tamanho, op)
    if(count != strlen(buf)+1) logexit("send"); // count = numero de bytes efetivamente transmitido

    memset(buf, 0, BUFSZ);
    unsigned total = 0;
    while(1){
        count = recv(s, buf + total, BUFSZ-total, 0);
        if(count != 0) break;
        total += count;
    }
    close(s);
    printf("recieved %u bytes\n", total);
    exit(EXIT_SUCCESS);

}