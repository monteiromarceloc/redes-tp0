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

void *connection_handler(void *port);

int main(int argc, char *argv[]) { 
	struct host hosts[MAXSIZE];
	struct server servers[MAXSIZE];
	int hcount=0, scount=0;

	pthread_t thread_id;
    char entrada[254];
    
    int port = atoi(argv[1]);

    // create thread for socket 
    if (pthread_create(&thread_id, NULL, connection_handler, (void*) port) < 0) {
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
				hostname[strlen(hostname)] = '\0'; // necessário para impedir um bug
				for(int i=0; i<hcount; i++){
					if(strcmp(hosts[i].name, hostname) == 0){
						printf("IP: %s\n", hosts[i].ip);
						didfound = 1;
						break;
					}
				}
				if(didfound==0){
					printf("Tentando conectar-se com outros servidores...\n");

				}
			}
		}
	}

	return 0; 
} 

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *port) {
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    listen(socket_desc, 3);
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
    puts("Connection accepted");
    
    //Get the socket descriptor
    size_t read_size;
    char *message, client_message[2000];

    //Receive a message from client
    while ((read_size = recv(client_sock, client_message, 10, NULL)) > 0) {
        //end of string marker
        client_message[read_size] = '\0';
        printf("Server receive: %s\n", client_message);

        //Send the message back to client
        write(client_sock, client_message, strlen(client_message));

        //clear the message buffer
        memset(client_message, 0, 2000);
    }

    if (read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    }  else if (read_size == -1) {
        perror("recv failed");
    }

    return 0;
}
