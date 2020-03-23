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

void recvMsg(const int r, char* buf, int size){
	printf("Aguardando msg...\n");
	memset(buf, 0, 10);
	size_t total = recv(r, buf, 10, 0);
	printf("Recebeu %d bytes\n", (int)total);
	printf("Tamanho recebido: %d\n", (int)strlen(buf));
	printf("%s\n", buf);
}

void sendMatricula(const int r, char * msg, int size){
	int intValue  = atoi(msg);
	// TODO break into bytes
	printf("Enviando %d...", intValue);
	char sendBuf[size];
	sprintf(sendBuf, "%d", intValue);
	ssize_t count;
	count = send(r, sendBuf, size, 0);
	printf("Enviou %d bytes\n", (int)count);
	if(count != size) logexit("send");
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
	char rBuf[5];
	recvMsg(s, rBuf, 5);

	// Enviar SENHA
	char senha[9];
	printf("senha> ");
	scanf ("%[^\n]%*c", senha);
	sendMsg(s, senha, 8);


	// Receber OK e Matricula
	char okBuf[2];
	recvMsg(s, okBuf, 2);
	char matBuf[9];
	recvMsg(s, matBuf, 9);

	// Enviar Matricula
	char matricula[11];
	printf("matricula> ");
	scanf ("%[^\n]%*c", matricula);
	sendMatricula(s, matricula, 10);
	recvMsg(s, okBuf, 2);
	exit(EXIT_SUCCESS);
}
