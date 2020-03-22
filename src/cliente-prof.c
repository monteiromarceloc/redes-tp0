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

void sendMsg(const int r, char * msg){
	printf("enviando %s...\n", msg);
	int size = strlen(msg);
	char buf[size];
	for (int i = 0; i < size; i++){
		buf[i] = msg[i];
	}
	send(r, buf, size, 0);
	printf("enviou\n");
}

int main(int argc, char **argv)
{
	// Identificando argumentos
	if(argc<=2) logexit("Falta inserir IP e porto como argumentos argv");
	char *IP = argv[1];
	int port = atoi(argv[2]);
	printf("IP: %s\nPorto: %d\n\n", IP, port);

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

	while(1){
		// Receber READY
		char buf[512];
		memset(buf, 0, 512);
		size_t total = recv(s, buf, 10, 0);
		printf("received %d bytes\n", (int)total);
		puts(buf);

		// Enviar SENHA
		// ssize_t count;
		char linha[512];
		printf("senha> ");
		fgets(linha, 511, stdin);
		sendMsg(s, linha);
		// count = send(s, linha, strlen(linha)+1, 0); // incluir o char \0
		// if(count != strlen(linha)+1) logexit("send");
	}


	// char buf[512];
	// memset(buf, 0, 512);
	// unsigned total = 0;
	// while(1) {
	// 	count = recv(s, buf+total, 512-total, 0);
	// 	if(count == 0) break;
	// 	total += count;
	// }

	// printf("received %d bytes\n", (int)total);
	// puts(buf);

	exit(EXIT_SUCCESS);
}

