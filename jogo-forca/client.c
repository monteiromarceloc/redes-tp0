#include "libs/common.h"
#include "libs/socketHandler.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

int main(int argc, char **argv) {
    int s = connectToServer(argc, argv);

    int tamanho = (int)recvByte(s);
    printf("tamanho: %d\n", tamanho);

    while(1){
        send2(s);
        int done = recvAnswer(s, tamanho+2);
        if(done == 1) break;
    }
    printf("Você conseguiu!\n");
    close(s);
    exit(EXIT_SUCCESS);
    return 0;
}