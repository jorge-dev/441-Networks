/*
Name: 	Jorge Avila
ID:		10123968
Asg#:	2
Tut#:	1
Aknowledments:   Code Written by Carey Williamson and modified by Jorge Avila
				 I modified the original code and added all the needed functionality 
				 to satisfy assignments requirement.
*/
/* Include files */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
//#include <bits/stdc++.h>
//#include <algorithm>

using namespace std;
/* Constants */
#define MAX_BUFFER_SIZE 40
#define PORT 1113

int main()
{
  // main variables and socket structure declaration
  struct sockaddr_in si_server, si_client;
  struct sockaddr *server, *client;
  int s, i = sizeof(si_server);
  socklen_t len = sizeof(si_server);

  //message related variables
  char messagein[MAX_BUFFER_SIZE];
  char messageout[MAX_BUFFER_SIZE];
  int readBytes;

  // initialize listening socket
  if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
    printf("Could not setup a socket!\n");
    return 1;
  }

  //initialize proxies
  memset((char *)&si_server, 0, sizeof(si_server));
  si_server.sin_family = AF_INET;
  si_server.sin_port = htons(PORT);
  si_server.sin_addr.s_addr = htonl(INADDR_ANY);
  server = (struct sockaddr *)&si_server;
  client = (struct sockaddr *)&si_client;

  //binding socket
  if (bind(s, server, sizeof(si_server)) == -1)
  {
    printf("Could not bind to port %d!\n", PORT);
    return 1;
  }

  fprintf(stderr, "You are in the Upper UDP server\n");
  printf("UDP server listening on port %d...\n", PORT);

  // infinite listening loop
  for (;;)
  {
    /* clear out message buffers to be safe */
    bzero(messagein, MAX_BUFFER_SIZE);
    bzero(messageout, MAX_BUFFER_SIZE);

    /* receive data from client */
    if ((readBytes = recvfrom(s, messagein, MAX_BUFFER_SIZE, 0, client, &len)) < 0)
    {
      printf("Read error!\n");
      return -1;
    }

    printf("  server received \"%s\" from IP %s port %d\n",
           messagein, inet_ntoa(si_client.sin_addr), ntohs(si_client.sin_port));

    /* create the outgoing message (as an ASCII string) */
    sprintf(messageout, "%s", messagein);
    //copy charr array to string and make the needed modifications
    std::string upper(messageout);

    // do the text transformation
    //  transform(upper.begin(), upper.end(), upper.begin(), std::toupper);

    for (auto & c: upper) c = toupper(c);
    
    // copy modified message to char array being sent to client
    strcpy(messageout, upper.c_str());

    printf("Server sending back the message: \"%s\"\n", messageout);

    /* send the result message back to the client */
    sendto(s, messageout, strlen(messageout), 0, client, len);
  }

  close(s);
  return 0;
}
