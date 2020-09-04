#include "libs/common.h"
#include "libs/socketHandler.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#define IPv 4
#define WORD "arara"

int main(int argc, char **argv) {
    int s = initSocketServer(argc, argv, IPv);
    unsigned int tam = strlen(WORD);
    
    while(1){
        int found = 0;
        unsigned char palpite;
        int pos[tam], count;
        memset(pos, -1, tam);

        int csock = acceptConnection(s);
        send1(csock, tam);

        while(1){
            palpite = (char)recvByte(csock);
            count = charFind(palpite, WORD, tam, pos);
            found += count;
            if (found >= tam) break;

            send3(csock, count, pos);
        }
        send4(csock);
        close(csock);
        printf("[log] connection closed. Waiting new connection\n");

    }
    exit(EXIT_SUCCESS);
    return 0;
}