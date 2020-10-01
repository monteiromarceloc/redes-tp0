// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXSIZE 100
#define STRSIZE 50
#define MAXLINE 200

struct host
{
	char name[STRSIZE];
	char ip[STRSIZE];
};

struct server
{
	char ip[STRSIZE];
	int porta;
};

struct host hosts[MAXSIZE];
struct server servers[MAXSIZE];
int hcount, scount;

void runCommand(char *line);
void searchForIP(char *hostname);
void *response_handler(int port);
const char *request_handler(int port, char *hostname);

int main(int argc, char *argv[])
{
	hcount = 0;
	scount = 0;
	// Cria thread para receber conexões socket UDP
	int port = atoi(argv[1]);
	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, response_handler, (void *)port) < 0)
	{
		perror("could not create thread");
		return 1;
	}

	// Le arquivo, se houver
	if (argc > 2)
	{
		FILE *fp;
		char *line = NULL;
		size_t len = 0;
		ssize_t read;

		fp = fopen(argv[2], "r");
		if (fp == NULL)
			exit(EXIT_FAILURE);

		while ((read = getline(&line, &len, fp)) != -1)
		{
			runCommand(line);
		}

		fclose(fp);
		if (line)
			free(line);
	}

	// Recebe comandos do teclado
	char entrada[MAXLINE];
	while (1)
	{
		fgets(entrada, MAXLINE, stdin);
		entrada[strcspn(entrada, "\n")] = '\0'; // Para remover o caracter \n
		runCommand(entrada);
	}

	return 0;
}

void runCommand(char *line)
{
	char *comando = strtok(line, " ");

	if (strcmp(comando, "add") == 0)
	{
		char *name = strtok(NULL, " ");
		char *ip = strtok(NULL, " ");
		if (name == NULL || ip == NULL)
		{
			printf("Usage: add <hostname> <ip>\n");
		}
		else
		{

			strcpy(hosts[hcount].name, name);
			strcpy(hosts[hcount].ip, ip);
			hcount++;
			printf("Host %s adicionado com sucesso.\n", name);
		}
	}

	else if (strcmp(comando, "link") == 0)
	{
		char *ip = strtok(NULL, " ");
		char *porta = strtok(NULL, " ");
		if (porta == NULL || ip == NULL)
		{
			printf("Usage: link <ip> <porta>\n");
		}
		else
		{
			strcpy(servers[scount].ip, ip);
			servers[scount].porta = atoi(porta);
			scount++;
			printf("Servidor adicionado com sucesso.\n");
		}
	}

	else if (strcmp(comando, "search") == 0)
	{
		char *hostname = strtok(NULL, " ");
		if (hostname == NULL)
			printf("Usage: search <hostname>\n");
		else
			searchForIP(hostname);
	}
}

void searchForIP(char *hostname)
{
	int didfound = 0, didRecv = 0;
	hostname[strlen(hostname)] = '\0';
	for (int i = 0; i < hcount; i++)
	{
		if (strcmp(hosts[i].name, hostname) == 0)
		{
			printf("IP: %s\n", hosts[i].ip);
			didfound = 1;
			break;
		}
	}
	if (didfound == 0)
	{
		printf("Tentando conectar-se com outros servidores...\n");
		for (int i = 0; i < scount; i++)
		{
			const char *res = request_handler(servers[i].porta, hostname);
			if (strcmp(res, "-1") != 0 && strcmp(res, "-1") != 1)
			{
				printf("IP: %s\n", res);
				didRecv = 1;
				break;
			}
		}
		if (didRecv == 0)
		{
			printf("Host not found\n");
		}
	}
}

const char *request_handler(int port, char *hostname)
{
	int sockfd;
	char buffer[MAXLINE];

	struct sockaddr_in servaddr;

	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = INADDR_ANY;

	int n;
	socklen_t len;

	sendto(sockfd, (const char *)hostname, strlen(hostname),
				 0, (const struct sockaddr *)&servaddr,
				 sizeof(servaddr));

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 1000;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
	{
		perror("Error");
	}
	n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);

	if (n < 0)
	{ //timeout
		printf("Timout reached\n");
		return "-1";
	}
	close(sockfd);
	buffer[n] = '\0';
	return buffer;
}

void *response_handler(int port)
{
	int sockfd;
	char buffer2[MAXLINE];
	struct sockaddr_in servaddr, cliaddr;

	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);

	// Bind the socket with the server address
	if (bind(sockfd, (const struct sockaddr *)&servaddr,
					 sizeof(servaddr)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		int n;
		socklen_t len;

		len = sizeof(cliaddr); //len is value/resuslt

		n = recvfrom(sockfd, (char *)buffer2, MAXLINE,
								 MSG_WAITALL, (struct sockaddr *)&cliaddr,
								 &len);
		buffer2[n] = '\0';
		printf("Client: %s\n", buffer2);

		int didfound = 0;
		for (int i = 0; i < hcount; i++)
		{
			if (strcmp(hosts[i].name, buffer2) == 0)
			{
				printf("IP found: %s\n", hosts[i].ip);
				sendto(sockfd, (const char *)hosts[i].ip, strlen(hosts[i].ip),
							 0, (const struct sockaddr *)&cliaddr,
							 len);
				didfound = 1;
				break;
			}
		}
		if (didfound == 0)
		{
			// TODO: search others

			unsigned char res[2] = "-1";
			sendto(sockfd, (const char *)res, strlen(res),
						 0, (const struct sockaddr *)&cliaddr,
						 len);
		}
	}
}
