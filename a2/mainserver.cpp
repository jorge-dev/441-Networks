/*
Name: 	Jorge Avila
ID:		10123968
Asg#:	2
Tut#:	1
Aknowledments:   Code Written by Carey Williamson and modified by Jorge Avila
				 I modified the original code and added all the needed functionality 
				 to satisfy assignments requirement.
*/

/* Include files for C socket programming and stuff */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <array>
using namespace std;

/* Global manifest constants */
#define MAX_MESSAGE_LENGTH 1000
#define MYPORTNUM 4441
#define IDENTITY_PORT 1111
#define REVERSE_PORT 1112
#define UPPER_PORT 1113
#define LOWER_PORT 1114
#define CEASAR_PORT 1115
#define YOURS_PORT 1116

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
void clientUdp(int portNum, char ipAddress[], char buf[])
{
	int MAX_BUFFER_SIZE = MAX_MESSAGE_LENGTH;
	struct sockaddr_in si_server;
	struct sockaddr *server;
	int s, i = sizeof(si_server);
	socklen_t len = sizeof(si_server);
	int readBytes;
	char connectionError[MAX_MESSAGE_LENGTH] = "=ERR";

	// Time interval initialization variables
	struct timeval tv;
	//seconds
	tv.tv_sec = 0;
	//micro-seconds
	tv.tv_usec = 500000; // wait half a second

	//Initialize socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		printf("Could not set up a socket!\n");
	}
	//Initialize proxy
	memset((char *)&si_server, 0, sizeof(si_server));
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(portNum);
	server = (struct sockaddr *)&si_server;

	//Convert IPv4 address into network address structure
	if (inet_pton(AF_INET, ipAddress, &si_server.sin_addr) == 0)
	{
		printf("inet_pton() failed\n");
	}

	fprintf(stderr, "Youre inside the UPD client\n");

	// set a timeout for sendind/receiving data from udp server
	if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
	{
		perror("Error");
	}

	//Try to send data to server
	if (sendto(s, buf, strlen(buf), 0, server, sizeof(si_server)) == -1)
	{
		printf("Failed to send data to server\n");
	}

	//Try to receive data from server
	if ((readBytes = recvfrom(s, buf, MAX_BUFFER_SIZE, 0, server, &len)) == -1)
	{
		printf("Failed to read data from server after timeout!\n");
		strcpy(buf, connectionError);
		printf("the string is: %s\n", buf);
	}
	else
		buf[readBytes] = '\0'; // proper null-termination of string

	
	close(s);
}


int main()
{
	//Main variables and socket struct initialization
	struct sockaddr_in server;
	static struct sigaction act;
	char messagein[MAX_MESSAGE_LENGTH];
	char messageout[MAX_MESSAGE_LENGTH];
	char udpServerAddress[MAX_MESSAGE_LENGTH] = "127.0.0.1";
	array<int, 20> sequence;
	int parentsockfd;
	int i, j;
	int pid;
	char c;
	string getMessage, getSequence;

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
				//find delimeter and separate message from sequence
				string splitMessage(messagein);
				string delimiter = "\n\n";
				size_t pos = 0;
				pos = splitMessage.find(delimiter);
				//get message
				getMessage = splitMessage.substr(0, pos);
				//get sequence
				getSequence = splitMessage.erase(0, pos + delimiter.length());

				//store sequence numbers into int array
				for (int i = 0; i < getSequence.length(); i++)
				{
					sequence[i] = getSequence[i] - '0';
				}
				// cout << "number of items in sequence:: " << getSequence.length() << endl;

				strcpy(messagein, getMessage.c_str());

				/* print out the received message */
				printf("Child process received word: %s\n", messagein);
				printf("That word has %zu characters!\n", strlen(messagein));
				// cout << "this is the sequence: " << getSequence << endl;

				// /* create the outgoing message (as an ASCII string) */
				sprintf(messageout, "%s", messagein);

				//create a temp message array to store message in and its modifications
				char tempMsg[MAX_MESSAGE_LENGTH];
				strcpy(tempMsg, getMessage.c_str());
				// loop through all the sequence of transformations and apply this modifications to the word
				for (int i = 0; i < getSequence.size(); i++)
				{
					//stores current sequence number
					int serverOption = sequence[i];

					// send data to appropiate server depending on the sequence number
					switch (serverOption)
					{
					case 1:
						clientUdp(IDENTITY_PORT, udpServerAddress, tempMsg);
						break;

					case 2:
						clientUdp(REVERSE_PORT, udpServerAddress, tempMsg);
						break;

					case 3:
						clientUdp(UPPER_PORT, udpServerAddress, tempMsg);
						break;

					case 4:
						clientUdp(LOWER_PORT, udpServerAddress, tempMsg);
						break;

					case 5:
						clientUdp(CEASAR_PORT, udpServerAddress, tempMsg);
						break;

					case 6:
						clientUdp(YOURS_PORT, udpServerAddress, tempMsg);
						break;

					default:
						printf("Sorry something wnet worng. You shoudn't be seeing this\n");
						break;
					}
				}
				//copy the modified word or sentence into messageout
				strcpy(messageout, tempMsg);

				//Print the actual message the server is sending to client
				printf("Child about to send message: %s\n", messageout);

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
