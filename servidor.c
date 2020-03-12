#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

void logexit(const char *str) {
	// perror(str);
  printf("Error: %s", str);
	exit(EXIT_FAILURE);
}

int main() {
  // TODO: Gera senha prof e alunos

  // Abre conecção
  int s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s == -1) logexit("socket");

  // TODO: setsockopt(...) para a renovar a porta

  struct in_addr inaddr;
	inet_pton(AF_INET, "127.0.0.1", &inaddr);

	struct sockaddr_in addr;
	struct sockaddr *addrptr = (struct sockaddr *)&addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(51511);
	addr.sin_addr = inaddr;

	if(bind(s, addrptr, sizeof(struct sockaddr_in))) logexit("bind");
	if(listen(s, 10)) logexit("listen"); // aceita até 10 conexões

	printf("Esperando conexão\n");

  while(1) {
    // dá pra por esses caras fora do while?
    struct sockaddr_in raddr;
		struct sockaddr *raddrptr = (struct sockaddr *)&raddr;
		socklen_t rlen = sizeof(struct sockaddr_in);

    // TODO: usar threads igual no server-mt, usar pull
    // usar pull de threads: cria x threads e aloca novos clientes,
    // se lotar, coloca numa fila.

		int r = accept(s, raddrptr, &rlen);
		if(r == -1) logexit("accept");
    // TODO: adicionar timeout no r
		char buf[512];
		char ipcliente[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(raddr.sin_addr),
				ipcliente, 512);

		printf("conexao de %s %d\n", ipcliente,
				(int)ntohs(raddr.sin_port));

		size_t c = recv(r, buf, 512, 0);
		printf("recebemos %d bytes\n", (int)c);
		puts(buf);

		sprintf(buf, "seu IP eh %s %d\n", ipcliente,
				(int)ntohs(raddr.sin_port));
		printf("enviando %s\n", buf);

		send(r, buf, strlen(buf)+1, 0);
		printf("enviou\n");

		close(r);
	}

	exit(EXIT_SUCCESS);
  return 0;
}