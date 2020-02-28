/* Simple program to demonstrate a UDP-based server.

 * 
 * Compile using "cc -o wordlen-UDPserver wordlen-UDPserver.c"
 */

/* Include files */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
using namespace std;

/* constants */
#define MAX_BUFFER_SIZE 40
#define PORT 1115


// Ceasar cypher function
// I found this function here https://www.geeksforgeeks.org/caesar-cipher-in-cryptography/
// however i made some modifications to match the assignment requirements
string cypher(string text)
{
  int shift =13;
  string result = "";
  size_t found;
  // traverse text
  string letters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

  for (int i = 0; i < text.length(); i++)
  {
    found = letters.find(tolower(text[i]));
    if (found != string::npos)
    {
      // apply transformation to each character
      // Encrypt Uppercase letters
      if (isupper(text[i]))
        result += char(int(text[i] + shift - 65) % 26 + 65);

      // Encrypt Lowercase letters
      else
        result += char(int(text[i] + shift - 97) % 26 + 97);
    }
    else
    {
      result += text[i];
    }
  }

  // Return the resulting string
  return result;
}


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

//initializing proxies
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
  fprintf(stderr, "You are in the Ceasar UDP server\n");
  printf("UDP server listening on port %d...\n", PORT);

  /* infinite listening loop*/
  for (;;)
  {
    /* clear the message at the start of every iteration */
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
    string messageStr(messageout);
    
    //cypher the message
    messageStr = cypher(messageStr);

    // copy modified message to char array being sent to client
    strcpy(messageout, messageStr.c_str());

    printf("Server sending back the message: \"%s\"\n", messageout);


    /* send modified message back to the client */
    sendto(s, messageout, strlen(messageout), 0, client, len);
  }

  close(s);
  return 0;
}
