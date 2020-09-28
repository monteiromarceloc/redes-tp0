// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define PORT 8080
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

int main() { 
	struct host hosts[MAXSIZE];
	struct server servers[MAXSIZE];
	int hcount=0, scount=0;

	while(1){
		char entrada[MAXLINE];
		fgets(entrada, MAXLINE, stdin);
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
				char * aux = malloc(strlen(hostname));
				strcpy(aux, hostname);
				aux[strlen(hostname)-1] = '\0'; // necessário para impedir um bug
				for(int i=0; i<hcount; i++){
					if(strcmp(hosts[i].name, aux) == 0){
						printf("IP: %s\n", hosts[i].ip);
						didfound = 1;
						break;
					}
				}
				if(didfound==0){
					printf("IP ainda não encontrado\n");
				}
			}
		}
	}

	return 0; 
} 
