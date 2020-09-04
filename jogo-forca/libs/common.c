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

void send2(int s) {
    printf("\npalpite> ");
    unsigned char msg[TAM]; 
    msg[0] = 2;
    msg[1] = getchar();
    getchar(); // must be here due to the '\n'
    int count = send(s, msg, TAM, 0);
    if(count != TAM) logexit("send");
}

void send3(int s, int count, int* pos) {
    unsigned char msg[2+count]; 
    msg[0] = 3;
    msg[1] = count;
    for(int i=2; i<2+count; i++)
        msg[i]=pos[i];
    int count2 = send(s, msg, 2+count, 0);
    if(count2 != 2+count) logexit("send");
}

void send4(int s) {
    printf("Vai acabar\n");
    unsigned char msg[1];
    msg[0] = 4;
    int count = send(s, msg, 1, 0); // TODO: something is wrong here
    printf("game over send: %d", count);
    if(count != 1) logexit("send");
    printf("Acabou!\n");
}

unsigned char recvByte(int s){
    unsigned char buf[TAM]; 
    memset(buf, 0, TAM);
    size_t count = recv(s, buf, TAM, 0);
    if(TAM != (int)count) logexit("recvByte");
    // printf("recieved %d bytes: %u %u\n", (int)count, buf[0], buf[1]);
    return buf[1];
}

int recvAnswer(int s, int max){
    unsigned char buf[max]; 
    memset(buf, 0, max);
    ssize_t count = recv(s, buf, max, 0);
    if((int)count == 0) return 1;
    printf("count: %d tipo: %u ocorrencias: %u\n", (int)count, buf[0], buf[1]);
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
