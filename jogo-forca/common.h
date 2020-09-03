#pragma once

#include <stdlib.h>
#include <arpa/inet.h>

void logexit(const char *str);

void send1(int s, unsigned int tam);

void send2(int s);

void recvBytes(int s);


int addrparse(const char*addrstr, const char *portstr, struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init(const char *proto, const char *portstr, struct sockaddr_storage *storage);
