Computer Network - CPSC 441
Worksheet 1 - Send / Receive messages
________________


/*
 * A simple TCP client that send messages to a Google server and display the message
   from the server.
 */


#include <stdio.h> // for printf
#include <stdlib.h> // for atoi() and exit()
#include <sys/socket.h> // for socket(), connect(), send(), and recv()
#include <arpa/inet.h> // for sockaddr_in and inet_addr()
#include <string.h> // for memset()
#include <unistd.h> // close()


const int BUFFERSIZE = 1024;   // Size the message buffers


int main(int argc, char const *argv[]) {


  // Create a TCP socket
  int  socket_info; // A socket descriptor


   // * AF_INET: using address family "Internet Protocol address"
  // * SOCK_STREAM: Provides sequenced, reliable, bidirectional, connection-mode byte streams.
  // * IPPROTO_TCP: TCP protocol
  socket_info = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


  // Initialize the server information
  struct sockaddr_in server;   // Address of the server
  server.sin_addr.s_addr = inet_addr("172.217.14.195"); // Server IP
  server.sin_family = AF_INET; // Use Internet address family
  server.sin_port = htons(80); // Server port number


  // Connect to the server
  // * sock: the socket for this connection
  // * serverAddr: the server address
  // * sizeof(*): the size of the server address
  if (connect(socket_info, (struct sockaddr *)&server, sizeof(server)) < 0){
  printf("Fail in connecting.\n");
  exit(0);
  }
  printf("Connected.\n");


  // Send the message to the server
  char *message = "GET / HTTP/1.1\r\n\r\n";
  if (send(socket_info, message, strlen(message), 0)<0){
    printf("Fail in message sending\n");
    exit(0);
  }
  printf("Message sent.\n");


  // Receiving Message
  char reply[BUFFERSIZE];
  if (recv(socket_info, reply, BUFFERSIZE, 0)<0){
    printf("Fail in receiving data.\n");
    exit(0);
  }


  printf("Reply received.\n");
  printf("%s\n", reply);


  memset(&reply, 0, BUFFERSIZE);
  close(socket_info);


  return 0;
}