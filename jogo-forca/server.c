#include "libs/common.h"
#include "libs/socketHandler.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#define IPv 4
#define WORD "arara" // TODO: handle casesensitiveness

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
            printf("palpite: %c\n", palpite);
            count = charFind(palpite, WORD, tam, pos);
            found += count;
            if (found >= tam) {
                send4(csock);
                close(csock);
                break;
            }
            send3(csock, count, pos);
        }
    }
    exit(EXIT_SUCCESS);
    return 0;
}