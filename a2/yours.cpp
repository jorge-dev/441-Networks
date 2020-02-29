/* Simple program to demonstrate a UDP-based server.
 * Loops forever.
 * Receives a word from the client. 
 * Sends back the length of that word to the client.
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

/* Constants */
#define MAX_BUFFER_SIZE 40
#define PORT 1116

// XOR encryption/decryption
// This will encrypt and decrypt a given message if message was encrypted by this function
// I found this function here https://kylewbanks.com/blog/Simple-XOR-Encryption-Decryption-in-Cpp
string encryptDecrypt(string text)
{
  //keys used for encryption/decryption
  char key[20] = {'Y', 'O', 'U', 'C', 'A','N','T','H','A','C','K','T','H','I','S'}; 
    string output = text;
    
    for (int i = 0; i < text.size(); i++)
        //XOR encryption
        output[i] = text[i] ^ key[i % (sizeof(key) / sizeof(char))];
    
    return output;
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

  fprintf(stderr, "You are in the Encrypt UDP server. \n");
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
    string messageStr(messageout);

    //encryptDecrypt message
    messageStr = encryptDecrypt(messageStr);

    // copy modified message to char array being sent to client
    strcpy(messageout, messageStr.c_str());

    printf("Server sending back the message: \"%s\"\n", messageout);

    /* send the result message back to the client */
    sendto(s, messageout, strlen(messageout), 0, client, len);
  }

  close(s);
  return 0;
}
