#pragma once

#include <stdlib.h>
#include <arpa/inet.h>

void logexit(const char *str);

void sendMsg(int s, char *msg);

void recvMsg(int s);

void getAndSendMsg(int s);

int addrparse(const char*addrstr, const char *portstr, struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init(const char *proto, const char *portstr, struct sockaddr_storage *storage);
