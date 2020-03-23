#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

void logexit(const char *str){
	printf("Error: %s", str);
	perror(str);
	exit(EXIT_FAILURE);
}

void sendMsg(const int r, char * msg, int size){
	printf("Enviando %s...\n", msg);
	char sendBuf[size];
	for (int i = 0; i < size; i++){
		sendBuf[i] = msg[i];
	}
	ssize_t count;
	count = send(r, sendBuf, size, 0);
	printf("Enviou %d bytes\n", (int)count);
	if(count != size) logexit("send");
}

void recvMsg(const int r, char* recvBuf){
	printf("Aguardando msg...\n");
	memset(recvBuf, 0, 10);
	size_t total = recv(r, recvBuf, 10, 0);
	printf("Recebeu %d bytes\n", (int)total);
	printf("Tamanho recebido: %d\n", (int)strlen(recvBuf));
	printf("%s\n", recvBuf);
}

void recvList(const int r, char* recvBuf){
	printf("Aguardando msg...\n");
	// memset(recvBuf, 0, 512);
	unsigned total = 0;
	ssize_t count;
	while(1) {
		count = recv(r, recvBuf+total, 512-total, 0);
		printf("%s\n", recvBuf);
		if(count == 0 || strstr(recvBuf, "\0") != NULL) break;
		total += count;
	}
	printf("Recebeu %d bytes\n", (int)total);
}

int main(int argc, char **argv)
{
	// Identificando argumentos
	if(argc<=2) logexit("Falta inserir IP e porto como argumentos argv");
	char *IP = argv[1];
	int port = atoi(argv[2]);

	// Inicializando conexão
	int s;
	struct in_addr inaddr;
	inet_pton(AF_INET, IP, &inaddr);

	struct sockaddr_in addr;
	struct sockaddr *addrptr = (struct sockaddr *)&addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = inaddr;

	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s == -1) logexit("socket");

	if(connect(s, addrptr, sizeof(struct sockaddr_in)))
		logexit("connect");

	// Receber READY
	char buf[10];
	recvMsg(s, buf);

	// Enviar SENHA
	char senha[10];
	printf("senha> ");
	scanf ("%[^\n]%*c", senha);
	sendMsg(s, senha, 8);

	// Recebe LISTA
	char lista_de_presenca[512];
	recvList(s, lista_de_presenca);
	sendMsg(s, "OK", 2);

	exit(EXIT_SUCCESS);
}
