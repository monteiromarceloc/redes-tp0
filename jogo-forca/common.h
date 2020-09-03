#pragma once

#include <stdlib.h>
#include <arpa/inet.h>

void logexit(const char *str);

void sendByte(int s);

void recvBytes(int s);

void sendMsg(int s, char *msg);

void getAndSendMsg(int s);

void serverRecv(int s);

void recvMsg(int s);

int charToInt(char c);

int addrparse(const char*addrstr, const char *portstr, struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init(const char *proto, const char *portstr, struct sockaddr_storage *storage);
