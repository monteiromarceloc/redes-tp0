#include "libs/common.h"
#include "libs/socketHandler.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#define WORD "abca" // TODO: handle casesensitiveness

int main(int argc, char **argv) {
    int s = initSocketServer(argc, argv);
    
    while(1){
        int csock = acceptConnection(s);
        
        int found = 0;
        unsigned int tam = strlen(WORD);
        unsigned char palpite;
        int pos[tam], count;

        send1(csock, tam);
        while(1){
            palpite = (char)recvByte(csock);
            printf("palpite: %c\n", palpite);
            count = charFind(palpite, WORD, tam, pos);
            found += count;
            if (found >= tam) {
                send4(s);
                close(csock);
                break;
            }
            send3(csock, count, pos);
        }
    }
    exit(EXIT_SUCCESS);
    return 0;
}