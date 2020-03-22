#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

void logexit(const char *str){
	printf("Error: %s\n", str);
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

struct dados {
	int sock;
	struct sockaddr_in addr;
};

void * client_thread(void *param) {

	// Estabelecendo conexão
	pthread_t tid = pthread_self();
	struct dados *dd = param;
	int r = dd->sock;

	char ipcliente[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(dd->addr.sin_addr),
			ipcliente, 512);
	
	printf("thread %x para conexao de %s %d\n", (unsigned int)tid, ipcliente, (int)ntohs(dd->addr.sin_port));

	sendMsg(r, "READY");
	// size_t c = recv(r, buf, 512, 0);
	// printf("recebemos %d bytes\n", (int)c);
	// puts(buf);

	// sprintf(buf, "seu IP eh %s %d\n", ipcliente,
	// 		(int)ntohs(dd->addr.sin_port));
	// printf("enviando %s\n", buf);

	// send(r, buf, strlen(buf)+1, 0);
	// printf("enviou\n");

	close(r);
	pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	// Identificando argumentos
	if(argc==1) logexit("Falta inserir porto como argumento argv");
	int port = atoi(argv[1]);
	printf("Porto: %d\n\n", port);

	// Inicializar senhas

	// Inicializar socket
	int s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s == -1) logexit("socket");

	struct in_addr inaddr;
	inet_pton(AF_INET, "127.0.0.1", &inaddr);

	struct sockaddr_in addr;
	struct sockaddr *addrptr = (struct sockaddr *)&addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = inaddr;

	if(bind(s, addrptr, sizeof(struct sockaddr_in)))
		logexit("bind");

	if(listen(s, 10)) logexit("listen");
	printf("esperando conexao\n");

	while(1) {
		struct sockaddr_in raddr;
		struct sockaddr *raddrptr =
			(struct sockaddr *)&raddr;
		socklen_t rlen = sizeof(struct sockaddr_in);

		int r = accept(s, raddrptr, &rlen);
		if(r == -1) logexit("accept");

		struct dados *dd = malloc(sizeof(*dd));
		if(!dd) logexit("malloc");
		dd->sock = r;
		dd->addr = raddr;
		pthread_t tid;
		pthread_create(&tid, NULL, client_thread, dd);
	}

	exit(EXIT_SUCCESS);
}





