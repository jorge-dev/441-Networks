/* Name: Jorge Avila
   Tutorial: 01
   ID: 10123968
   Assignemt: 1

   **Clarification: This is the skeleton code Rachek Mclean provided
   in her tutorial. She is the author of 90% of this code except
   from the code inside the third loop which is the code I added to make the
   modifications to the web pages.
*/

/* 
 * Author: Rachel Mclean
 *
 */

/* standard libraries*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <regex>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

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
//#define PROXY_PORT 1224

/* string sizes */
#define MESSAGE_SIZE 2048

void dostuff(int); /* function prototype */
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int lstn_sock, data_sock, web_sock, pid;

void cleanExit(int sig)
{
	close(lstn_sock);
	close(data_sock);
	close(web_sock);
	exit(0);
}

int main(int argc, char *argv[])
{
	
	char client_request[MESSAGE_SIZE], server_request[MESSAGE_SIZE], server_response[10 * MESSAGE_SIZE], client_response[10 * MESSAGE_SIZE];
	char url[MESSAGE_SIZE], host[MESSAGE_SIZE], path[MESSAGE_SIZE];
	int clientBytes, serverBytes, i;

	// port given by user as argument
	int PROXY_PORT = atoi(argv[1]);

	/* to handle ungraceful exits */
	signal(SIGTERM, cleanExit);
	signal(SIGINT, cleanExit);

	//initialize proxy address
	printf("Initializing proxy address...\n");
	struct sockaddr_in proxy_addr;
	proxy_addr.sin_family = AF_INET;
	proxy_addr.sin_port = htons(PROXY_PORT);
	proxy_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//create listening socket
	printf("Creating lstn_sock...\n");
	lstn_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (lstn_sock < 0)
	{
		perror("socket() call failed");
		exit(-1);
	}

	//bind listening socket
	printf("Binding lstn_sock...\n");
	if (bind(lstn_sock, (struct sockaddr *)&proxy_addr, sizeof(struct sockaddr_in)) < 0)
	{
		perror("bind() call failed");
		exit(-1);
	}

	//listen for client connection requests
	printf("Listening on lstn_sock...\n");
	if (listen(lstn_sock, 20) < 0)
	{
		perror("listen() call failed...\n");
		exit(-1);
	}

	//infinite while loop for listening
	while (1)
	{
		printf("Accepting connections...\n");

		//accept client connection request
		data_sock = accept(lstn_sock, NULL, NULL);
		if (data_sock < 0)
		{
			perror("accept() call failed\n");
			exit(-1);
		}

		 pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(lstn_sock);
            //  dostuff(data_sock);
             exit(0);
         }
         else close(data_sock);

		//while loop to receive client requests
		while ((clientBytes = recv(data_sock, client_request, MESSAGE_SIZE, 0)) > 0)
		{
			printf("%s\n", client_request);

			//copy to server_request to preserve contents (client_request will be damaged in strtok())
			strcpy(server_request, client_request);

			//tokenize to get a line at a time
			char *line = strtok(client_request, "\r\n");

			//extract url
			sscanf(line, "GET http://%s", url);

			//separate host from path
			for (i = 0; i < strlen(url); i++)
			{
				if (url[i] == '/')
				{
					strncpy(host, url, i);
					host[i] = '\0';
					break;
				}
			}
			bzero(path, MESSAGE_SIZE);
			strcat(path, &url[i]);

			printf("Host: %s, Path: %s\n", host, path);

			//initialize server address
			printf("Initializing server address...\n");
			struct sockaddr_in server_addr;
			struct hostent *server;
			server = gethostbyname(host);

			bzero((char *)&server_addr, sizeof(struct sockaddr_in));
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(HTTP_PORT);
			bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);

			//create web_socket to communicate with web_server
			web_sock = socket(AF_INET, SOCK_STREAM, 0);
			if (web_sock < 0)
			{
				perror("socket() call failed\n");
			}

			//send connection request to web server
			if (connect(web_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0)
			{
				perror("connect() call failed\n");
			}

			//send http request to web server
			if (send(web_sock, server_request, MESSAGE_SIZE, 0) < 0)
			{
				perror("send(0 call failed\n");
			}

			//receive http response from server
			serverBytes = 0;

			int htmlPosition;

			while ((serverBytes = recv(web_sock, server_response, MESSAGE_SIZE, 0)) > 0)
			{
				//////////////////////////////////////////////////////////////////////////
				// Modify response... //
				//My code starts here 
				//////////////////////////////////////////////////////////////////////////

				// copy the content from server response into a string for easier manipulation of contents
				string modify(server_response);
				
				//Regular expressions to look for target string neeed to be modified
				regex floppy("[^\\w+-]Floppy");
				regex italy("[^\\w+-][iI]taly");
				regex image("src=[\"|\'](?!.+Carey)(.+?)[\"|\']");
				regex ignore("Carey-Italy.jpg");
				regex contentLen("(Content-Length:.*)(.*)");
				regex link("Cute-Floppy");
				regex html("<html>");
				regex text("This is a basic text file");
				smatch m; // match flag
		
				// only run this code once for every request
				// used to look for the size in bytes of the substring containing the html
				// and modifying the Content Length attribute in order to make sure 
				// the length of the modified string is exactly whar the server needs in order
				// to correctly display the contents 
				if (regex_search(modify, m, html) || regex_search(modify, m, text))
				{
					modify = regex_replace(modify, floppy, " Trolly");
					// modify = regex_replace(modify, italy, " Japan");
					// this is the line i would change to show for the bonus marks
					modify = regex_replace(modify, italy, " Germany");
					modify = regex_replace(modify, image, "src=./trollface.jpg");
					modify = regex_replace(modify, link, "trollface");

					// code to target the substring get its starting position in the array
					// and its full size in bytes in order to modify the Content Length sent back
					htmlPosition = modify.find("charset=UTF-8");
					string strbytes = modify.substr(htmlPosition + 16);
					int bytes = strbytes.length();
					string modBytes = "Content-Length: " + to_string(bytes);
					modify = regex_replace(modify, contentLen, modBytes);

				}
				//copy the contents of the modifies string back to server response which is a 
				// char [].
				strcpy(server_response, modify.c_str());

				//////////////////////////////////////////////////////////////////////////
				// Response Modified... 
				//My code ends here 
				//////////////////////////////////////////////////////////////////////////


				//we are not modifying here, just passing the response along
				printf("%s\n", server_response);
				bcopy(server_response, client_response, serverBytes);

				//send http response to client
				if (send(data_sock, client_response, serverBytes, 0) < 0)
				{
					perror("send() call failed...\n");
				}
				bzero(client_response, MESSAGE_SIZE);
				bzero(server_response, MESSAGE_SIZE);

			} //while recv() from server

		} //while recv() from client
		close(data_sock);
	} //infinite loop
	return 0;
}