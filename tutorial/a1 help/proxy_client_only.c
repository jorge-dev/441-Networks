/* 
 * Tutorial 3 - HTTP Proxy
 * Example: Simple Intercept Proxy (client <-> proxy only)
 * Created: Jan. 27, 2020
 * Author: Rachel Mclean
 *
 */

/* standard libraries*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* libraries for socket programming */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*libraries for parsing strings*/
#include <string.h>
#include <strings.h>

/*h_addr address*/
#include <netdb.h>

/*clean exit*/
#include <signal.h>

/* port numbers */
#define HTTP_PORT 80
#define PROXY_PORT 8080

/* string sizes */
#define MESSAGE_SIZE 2048

int lstn_sock, data_sock, web_sock;
char webpage[] = 
"HTTP/1.1 200 OK\r\n"
"Content-Length: 43\r\n\r\n"

"Your request was intercepted by the proxy\r\n";

void cleanExit(int sig){
	close(lstn_sock);
	close(data_sock);
	close(web_sock);
	exit(0);
}

int main(int argc, char* argv[]){
	char client_request[MESSAGE_SIZE], server_request[MESSAGE_SIZE], server_response[10*MESSAGE_SIZE], client_response[10*MESSAGE_SIZE];
	char url[MESSAGE_SIZE], host[MESSAGE_SIZE], path[MESSAGE_SIZE];
	int clientBytes, serverBytes, i;


    /* to handle ungraceful exits */
    signal(SIGTERM, cleanExit);
    signal(SIGINT, cleanExit);

    //initialize proxy address
	printf("Initializing proxy address...\n");
	struct sockaddr_in proxy_addr;
	proxy_addr.sin_family = AF_INET;
	proxy_addr.sin_port = htons(PROXY_PORT);
	proxy_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//creating lstn_sock
	printf("Creating lstn_sock...\n");
	lstn_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (lstn_sock <0){
		perror("socket() call failed");
		exit(-1);
	}

	//binding
	printf("Binding lstn_sock...\n");
	if (bind(lstn_sock, (struct sockaddr*)&proxy_addr, sizeof(struct sockaddr_in)) < 0){
		perror("bind() call failed");
		exit(-1);
	}

	//listening
	printf("Listening on lstn_sock...\n");
	if (listen(lstn_sock, 20) < 0){
		perror("listen() call failed...\n");
		exit(-1);
	}

	//infinite while loop for listening
	while (1){
		printf("Accepting connections...\n");

		//accept client connection request
		data_sock = accept(lstn_sock, NULL, NULL);
		printf("Creating data_sock...\n");
		if (data_sock < 0){
			perror("accept() call failed\n");
			exit(-1);
		}

		//while still receiving requests from the client
		while ((clientBytes = recv(data_sock, client_request, MESSAGE_SIZE, 0)) > 0){
			printf("%s\n", client_request);

			//////////////////////////////////////
			// Connect to server & send/receive //
			//////////////////////////////////////

			//since this program doesn't connect to server, we provide our own webpage
			strcpy(client_response, webpage); 

			//send webpage to client
			if (send(data_sock, client_response, MESSAGE_SIZE, 0) < 0){
				perror("send() call failed...\n");
			}
		}
		close(data_sock);

	}

	return 0;
}