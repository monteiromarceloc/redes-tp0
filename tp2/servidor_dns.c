// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define MAXLINE 50
#define MAXSIZE 100

struct host
{
    char * name;
    char * ip;
};

struct server
{
    char * ip;
    int porta;
};

struct host hosts[MAXSIZE];
struct server servers[MAXSIZE];
int hcount, scount;

void *response_handler(int port);
void request_handler(int port, char* hostname);

int main(int argc, char *argv[]) { 
    int port = atoi(argv[1]);
    char entrada[254];
	pthread_t thread_id;

    // create thread for socket 
    if (pthread_create(&thread_id, NULL, response_handler, (void*) port) < 0) {
        perror("could not create thread");
        return 1;
    }

    // continue listening to stdin
    while (1) {
        gets(entrada);
		char * comando = strtok(entrada, " ");

		if(strcmp (comando, "add") == 0) {
			char * name = strtok(NULL, " ");
			char * ip = strtok(NULL, " ");
			if(name == NULL || ip == NULL){
				printf("Usage: add <hostname> <ip>\n");
			} else {
				hosts[hcount].name = malloc(strlen(name)+1);
				hosts[hcount].ip = malloc(strlen(ip)+1);
				strcpy(hosts[hcount].name, name);
				strcpy(hosts[hcount].ip, ip);
				hcount++;
				printf("Host %s adicionado com sucesso.\n", name);
			}
		}

		else if (strcmp (comando, "link") == 0){
			char * ip = strtok(NULL, " ");
			char * porta = strtok(NULL, " ");
			if(porta == NULL || ip == NULL){
				printf("Usage: link <ip> <porta>\n");
			} else {
				servers[scount].ip = malloc(strlen(ip)+1);
				strcpy(servers[scount].ip, ip);
				servers[scount].porta = atoi(porta);
				scount++;
				printf("Servidor adicionado com sucesso.\n");
			}
		}

		else if (strcmp (comando, "search") == 0){
			char * hostname = strtok(NULL, " ");
			if(hostname == NULL){
				printf("Usage: search <hostname>\n");
			} else {
				int didfound = 0;
				hostname[strlen(hostname)] = '\0';
				for(int i=0; i<hcount; i++){
					if(strcmp(hosts[i].name, hostname) == 0){
						printf("IP: %s\n", hosts[i].ip);
						didfound = 1;
						break;
					}
				}
				if(didfound==0){
					printf("Tentando conectar-se com outros servidores...\n");
					for(int i=0; i<scount; i++){
						request_handler(servers[i].porta, hostname);
					}
				}
			}
		}
	}

	return 0; 
} 

void request_handler(int port, char* hostname) {
	int sockfd; 
	char buffer[MAXLINE]; 

	struct sockaddr_in	 servaddr; 

	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
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
		MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
			sizeof(servaddr)); 
	printf("Message sent.\n"); 
		
	n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
				MSG_WAITALL, (struct sockaddr *) &servaddr, 
				&len); 
	buffer[n] = '\0'; 
	printf("Server : %s\n", buffer); 

	close(sockfd); 
	return 0; 
}


void *response_handler(int port) {
    int sockfd; 
	char buffer[MAXLINE]; 
	struct sockaddr_in servaddr, cliaddr; 
	
	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
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
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	int n; 
	socklen_t len;

	len = sizeof(cliaddr); //len is value/resuslt 

	n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
				MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
				&len); 
	buffer[n] = '\0'; 
	printf("Client: %s\n", buffer); 

	int didfound = 0;
	for(int i=0; i<hcount; i++){
		// printf("searching... %lu %lu\n", strlen(hosts[i].name), strlen(buffer));
		if(strcmp(hosts[i].name, buffer) == 0){
			printf("IP found: %s\n", hosts[i].ip);
			sendto(sockfd, (const char *)hosts[i].ip, strlen(hosts[i].ip), 
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
					len); 
			didfound = 1;
			break;
		}
	}
	if(didfound==0){
		unsigned char * res[2];
		res[0]=2;
		res[1]=-1;
		sendto(sockfd, (const char *)res, strlen(res), 
			MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
				len);
}
	printf("Res message sent.\n"); 
}
