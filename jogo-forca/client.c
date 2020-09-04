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

    int tam = (int)recvByte(s);
    printf("\nNúmero de caracteres: %d\n", tam);
    char word[tam];
    memset(word,'_',tam);

    while(1){
        char palpite = (char)send2(s);
        int done = recvAnswer(s, tam, word, palpite);
        printf("%s\n", word);
        if(done == 1) break;
    }
    printf("Você conseguiu!\n");
    close(s);
    exit(EXIT_SUCCESS);
    return 0;
}