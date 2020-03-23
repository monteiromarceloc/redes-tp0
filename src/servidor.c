#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

// VARIÁVEIS GLOBAIS
char senha_prof[9], senha_aluno[9];
char lista_de_presenca[4][10];
int count_presentes;

struct dados {
	int sock;
	struct sockaddr_in addr;
};

// FUNÇÕES AUXILIARES
void logexit(const char *str){
	printf("Error: %s\n", str);
	perror(str);
	exit(EXIT_FAILURE);
}

void sendMsg(const int r, char * msg, int size){
	printf("Enviando %s...\n", msg);
	char buf[size];
	for (int i = 0; i < size; i++){
		buf[i] = msg[i];
	}
	ssize_t count;
	count = send(r, buf, size, 0);
	printf("Enviou %d bytes\n", (int)count);
	if(count != size) logexit("send");
}

void recvMsg(const int r, char* buf){
	printf("Aguardando msg...\n");
	memset(buf, 0, 10);
	size_t total = recv(r, buf, 10, 0);
	printf("Recebeu %d bytes\n", (int)total);
	printf("Tamanho recebido: %d\n", (int)strlen(buf));
	printf("%s\n", buf);
}

void randomString(char *str, int size){
	const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
	if (size) {
		size--;
		for (int n = 0; n < size; n++) {
			int key = rand() % (int) (sizeof charset - 1);
			str[n] = charset[key];
		}
		str[size]='\0';
	}
}

void formatList(char * formatedList){
	if (count_presentes > 0){
		strcpy(formatedList, lista_de_presenca[0]);
		strcat(formatedList, "\n");
		for (int i = 1; i < count_presentes-1; i++){
			strcat(formatedList, lista_de_presenca[i]);
			strcat(formatedList, "\n");
		}
		if (count_presentes > 1) strcat(formatedList, lista_de_presenca[count_presentes-1]);
	} else {
		strcpy(formatedList, "");
	}
	strcat(formatedList, "\0");
}

void * client_thread(void *param) {

	// Estabelecendo conexão
	pthread_t tid = pthread_self();
	struct dados *dd = param;
	int r = dd->sock;

	char ipcliente[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(dd->addr.sin_addr), ipcliente, 512);
	
	printf("\nThread %x para conexao de %s %d\n", (unsigned int)tid, ipcliente, (int)ntohs(dd->addr.sin_port));

	sendMsg(r, "READY", 5);

	char senhaBuf[9];
	recvMsg(r, senhaBuf);
	senhaBuf[8]='\0';
	int isProf = strcmp(senhaBuf, senha_prof);
	printf("isProf: %d\n", isProf);
	if (isProf == 0) {
		// PROF
		char formatedList[count_presentes*11];
		formatList(formatedList);
		sendMsg(r, formatedList, count_presentes*11);
		char OKbuf[3];
		recvMsg(r, OKbuf);
	}
	else {
		int isAluno = strcmp(senhaBuf, senha_aluno);
		printf("isAluno: %d\n", isAluno);
		if (isAluno == 0){
			// ALUNO
			sendMsg(r, "OK", 2);
			sendMsg(r, "MATRICULA", 9);
			char matriculaBuf[10];
			recvMsg(r, matriculaBuf);
			strcpy(lista_de_presenca[count_presentes], matriculaBuf);
			count_presentes++;
			sendMsg(r, "OK", 2);
		}
		else {
			printf("Other stuff");
		}
	}
	close(r);
	pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
	// Identificando argumentos
	if(argc==1) logexit("Falta inserir porto como argumento argv");
	int port = atoi(argv[1]);

	// Inicializar senhas
	srand ( time(NULL) );
	randomString(senha_prof, 9);
	printf("Senha prof:\t%s\n", senha_prof);
	randomString(senha_aluno, 9);
	printf("Senha aluno:\t%s\n", senha_aluno);
	// TODO senhas nõ estão aleatórias

	count_presentes = 0;

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
	printf("Esperando conexao\n");

	while(1) {
		struct sockaddr_in raddr;
		struct sockaddr *raddrptr = (struct sockaddr *)&raddr;
		socklen_t rlen = sizeof(struct sockaddr_in);

		int r = accept(s, raddrptr, &rlen);
		if(r == -1) logexit("accept");

		// Cria thread pra lidar com cliente

		struct dados *dd = malloc(sizeof(*dd));
		if(!dd) logexit("malloc");
		dd->sock = r;
		dd->addr = raddr;
		pthread_t tid;
		pthread_create(&tid, NULL, client_thread, dd);

	}

	exit(EXIT_SUCCESS);
}
