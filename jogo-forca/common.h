#pragma once

#include <stdlib.h>
#include <arpa/inet.h>

void logexit(const char *str);

void send1(int s, unsigned int tam);

void send2(int s);

void send3(int s, int count, int* pos);

void send4(int s);

unsigned char recvByte(int s);

int recvAnswer(int s, int max);

int charFind(char c, char* s, int len, int* pos);

int addrparse(const char*addrstr, const char *portstr, struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init(const char *proto, const char *portstr, struct sockaddr_storage *storage);
