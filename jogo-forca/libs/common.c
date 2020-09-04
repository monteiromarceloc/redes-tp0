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

char send2(int s) {
    printf("\npalpite> ");
    unsigned char msg[TAM]; 
    msg[0] = 2;
    msg[1] = getchar();
    getchar(); // must be here due to the '\n'
    int count = send(s, msg, TAM, 0);
    if(count != TAM) logexit("send");
    return msg[1];
}

void send3(int s, int count, int* pos) {
    unsigned char msg[2+count]; 
    msg[0] = 3;
    msg[1] = count;
    for(int i=0; i<count; i++){
        msg[2+i]=pos[i];
    }
    int count2 = send(s, msg, 2+count, 0);
    if(count2 != 2+count) logexit("send");
}

void send4(int s) {
    unsigned char msg[1]; 
    msg[0] = 4;
    int count = send(s, msg, 1, 0);
    if(count != 1) logexit("send");
}

unsigned char recvByte(int s){
    unsigned char buf[TAM]; 
    memset(buf, 0, TAM);
    size_t count = recv(s, buf, TAM, 0);
    if(TAM != (int)count) logexit("recvByte");
    // printf("recieved %d bytes: %u %u\n", (int)count, buf[0], buf[1]);
    return buf[1];
}

int recvAnswer(int s, int tam, char* word, char palpite){ // TODO: check buf[0]
    unsigned char buf0[1], buf1[1];
    
    recv(s, buf0, 1, 0); // TODO: check error at all send and recv
    if((int)buf0[0] == 4) {
        for(int i = 0; i <= tam; i++)
  		    if(word[i] == '_')
  			    word[i] = palpite;
        return 1;
    }

    recv(s, buf1, 1, 0);
    int occ = (int)buf1[0];

    unsigned char buf2[occ];
    memset(buf2, 0, occ);
    recv(s, buf2, occ, 0);
    
    for(int i=0; i<occ; i++){
        int atIndex = (int)buf2[i];
        word[atIndex] = palpite;
    }
    return 0;
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
