/* TCP-based server example of socket programming.    */
/* The server receives an input word (e.g., "dog")    */
/* and returns the length of the word (e.g., "3").    */
/*                                                    */
/* Usage: cc -o wordlen-TCPserver wordlen-TCPserver.c */
/*        ./wordlen-TCPserver                         */
/*                                                    */
/* Written by Carey Williamson       January 13, 2012 */

/* Include files for C socket programming and stuff */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <string>
using namespace std;
/* Global manifest constants */
#define MAX_MESSAGE_LENGTH 1000
#define MYPORTNUM 1221

/* Optional verbose debugging output */
#define DEBUG 1

/* Global variable */
int childsockfd;

/* This is a signal handler to do graceful exit if needed */
void catcher(int sig)
{
	close(childsockfd);
	exit(0);
}
void clientUdp(int portNum, char ipAddress [], char  buf [1000])
{
	int MAX_BUFFER_SIZE = 1000;
	struct sockaddr_in si_server;
	struct sockaddr *server;
	int s, i = sizeof(si_server);
	socklen_t len = sizeof(si_server);
	// char buf[MAX_BUFFER_SIZE];
	int readBytes;

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		printf("Could not set up a socket!\n");
	
	}

	memset((char *)&si_server, 0, sizeof(si_server));
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(portNum);
	server = (struct sockaddr *)&si_server;

	if (inet_pton(AF_INET, ipAddress, &si_server.sin_addr) == 0)
	{
		printf("inet_pton() failed\n");
	}

	fprintf(stderr, "Youre inside the UPD client\n");

		if (sendto(s, buf, strlen(buf), 0, server, sizeof(si_server)) == -1)
		{
			printf("sendto failed\n");
			
		}

		if ((readBytes = recvfrom(s, buf, MAX_BUFFER_SIZE, 0, server, &len)) == -1)
		{
			printf("Read error!\n");
			
		}
		buf[readBytes] = '\0'; // proper null-termination of string
		// string answer (buf) ;
		// bzero(buf, MAX_BUFFER_SIZE);

		// printf("Answer: That word has %s letters!\n", buf);
	// }
	close(s);
}

/* Main program for server */
int main()
{
	struct sockaddr_in server;
	static struct sigaction act;
	char messagein[MAX_MESSAGE_LENGTH];
	char messageout[MAX_MESSAGE_LENGTH];
	int parentsockfd;
	int i, j;
	int pid;
	char c;
	string message;

	/* Set up a signal handler to catch some weird termination conditions. */
	act.sa_handler = catcher;
	sigfillset(&(act.sa_mask));
	sigaction(SIGPIPE, &act, NULL);

	/* Initialize server sockaddr structure */
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(MYPORTNUM);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	/* set up the transport-level end point to use TCP */
	if ((parentsockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "wordlen-TCPserver: socket() call failed!\n");
		exit(1);
	}

	/* bind a specific address and port to the end point */
	if (bind(parentsockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1)
	{
		fprintf(stderr, "wordlen-TCPserver: bind() call failed!\n");
		exit(1);
	}

	/* start listening for incoming connections from clients */
	if (listen(parentsockfd, 5) == -1)
	{
		fprintf(stderr, "wordlen-TCPserver: listen() call failed!\n");
		exit(1);
	}

	/* initialize message strings just to be safe (null-terminated) */
	bzero(messagein, MAX_MESSAGE_LENGTH);
	bzero(messageout, MAX_MESSAGE_LENGTH);

	fprintf(stderr, "Welcome! I am the TCP version of the word length server!!\n");
	fprintf(stderr, "server listening on TCP port %d...\n\n", MYPORTNUM);

	/* Main loop: server loops forever listening for requests */
	for (;;)
	{
		/* accept a connection */
		if ((childsockfd = accept(parentsockfd, NULL, NULL)) == -1)
		{
			fprintf(stderr, "wordlen-TCPserver: accept() call failed!\n");
			exit(1);
		}

		/* try to create a child process to deal with this new client */
		pid = fork();

		/* use process id (pid) returned by fork to decide what to do next */
		if (pid < 0)
		{
			fprintf(stderr, "wordlen-TCPserver: fork() call failed!\n");
			exit(1);
		}
		else if (pid == 0)
		{
			/* the child process is the one doing the "then" part */

			/* don't need the parent listener socket that was inherited */
			close(parentsockfd);

			/* obtain the message from this client */
			while (recv(childsockfd, messagein, MAX_MESSAGE_LENGTH, 0) > 0)
			{
				/* print out the received message */
				printf("Child process received word: %s\n", messagein);
				printf("That word has %zu characters!\n", strlen(messagein));

				// /* create the outgoing message (as an ASCII string) */
				 sprintf(messageout, "%s", messagein);

#ifdef DEBUG
				
			
				int port =1112;
				char address [MAX_MESSAGE_LENGTH] = "127.0.0.1";
				clientUdp(port, address, messageout);
				// strcpy(messageout,message.c_str());
				printf("Child about to send message: %s\n", messageout);
#endif

				
				/* send the result message back to the client */
				send(childsockfd, messageout, strlen(messageout), 0);

				/* clear out message strings again to be safe */
				bzero(messagein, MAX_MESSAGE_LENGTH);
				bzero(messageout, MAX_MESSAGE_LENGTH);
			}

			/* when client is no longer sending information to us, */
			/* the socket can be closed and the child process terminated */
			close(childsockfd);
			exit(0);
		} /* end of then part for child */
		else
		{
			/* the parent process is the one doing the "else" part */
			fprintf(stderr, "Created child process %d to handle that client\n", pid);
			fprintf(stderr, "Parent going back to job of listening...\n\n");

			/* parent doesn't need the childsockfd */
			close(childsockfd);
		}
	}
}
