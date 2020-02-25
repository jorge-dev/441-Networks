/*
A simple program to convert hostname to IP address
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char **argv) {
  // Check the standard input
  if (argc < 2) {
   fprintf(stderr,"Usage: %s hostname\n", argv[0]);
   exit(1);
 }


 struct hostent *hp = gethostbyname(argv[1]);


 if (hp == NULL) { // if convert was failed
   fprintf(stderr,"gethostbyname() failed\n");
   exit(1);
 } else {
   printf("%s = ", hp->h_name);
   unsigned int i=0;
   while ( hp -> h_addr_list[i] != NULL) {
     printf( "%s ", inet_ntoa( *( struct in_addr*)( hp -> h_addr_list[i])));
     i++;
   }
   printf("\n");
   exit(0);
 }
}
________________
#include <iostream>
using namespace std;


int main() {
 string str = "This is a basic text file. It tells a simple story about my floppy-eared friend Floppy, who is from Italy. Floppy is a dog, I think. Without your proxy, you should be able to view this page just fine. With your proxy, this page should look a bit different, with all mentions of my friend Floppy from Italy being changed into something else.  The page should still be formatted properly.";
 cout << "Hello World!!\n";
}