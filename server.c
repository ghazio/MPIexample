/*
 * Swarthmore College, CS 87
 * Copyright (c) 2020 Swarthmore College Computer Science Department,
 * Swarthmore PA, Professor Tia Newhall
 */

/*
 * cs87talk server
 *
 * TODO: (your names, date)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include <stdbool.h>

#include "cs87talk.h"


#define BACKLOG 3

// define any types that are local to the server here
// define any global variables you want here

/*********************************************/
int main(int argc, char **argv) {

  int listenfd, confd, ret;
  struct sockaddr_in saddr, caddr;
  struct linger linger_val;
  int optval;
  unsigned int socklen;

  // step (0) init any server-side data structs

  // step (1) create a TCP socket
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if(listenfd == -1) {
    perror("Create Socket Failed\n");
    exit(1);
  }

  // set socket opts on the listen socket SO_LINGER off and SO_REUSEADDR
  // here is the code to do it (it is complete)
  linger_val.l_onoff = 0;
  linger_val.l_linger = 0;
  ret = setsockopt(listenfd, SOL_SOCKET, SO_LINGER, (void *)&linger_val,
      (socklen_t) (sizeof(struct linger)));
  if(ret < 0) {
    perror("Setting socket option failed");
    exit(1);
  }
  // set SO_REUSEADDR on a socket to true (1):
  optval = 1;
  ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  if(ret < 0) {
    perror("Setting socket option failed");
    exit(1);
  }

  // setp (2) bind to port CS87_TALKSERVER_PORTNUM
  // (a) create and init an address struct (this is complete)
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(CS87_TALKSERVER_PORTNUM);
  saddr.sin_addr.s_addr = INADDR_ANY;
  // TODO: uncomment bind call
   ret = bind(listenfd, (struct sockaddr *)&saddr, sizeof(saddr));
   if(ret < 0) {
     perror("Binding socket failed");
     exit(1);
   }
  // (b) call bind
  // (c) check and handle error


  // step (3) tell OS we are going to listen on this socket
  // BACKLOG specifies how much space the OS should reserve for incomming
  // connections that have not yet been accepted by this server.
  // TODO: uncomment listen call and add error handling
   ret = listen(listenfd, BACKLOG);
   if(ret < 0) {
     perror("Binding socket failed");
     exit(1);
   }


  socklen = sizeof(caddr);
  // now the server is ready to accept connections from clients

  // TODO: remove this debugging output (it is just for compilation)
  printf("socklen = %u sockaddr addr %u\n", socklen, saddr.sin_addr.s_addr);
  int open = 1;
  while(1) {

    // TODO: finish the server main loop
    // step (4) accept the next connectino from a client
     confd = accept(listenfd, (struct sockaddr *) &caddr, &socklen);
     if(confd<0){
       perror("accepting socket failed");
       exit(1);
     }



       char* name_client = malloc(sizeof(char)*(NAMEMAX));

       ret = receive_Hello(confd,&open,name_client);
       if(ret<0){
          close(confd);
          perror("Connection with client not established");
          exit(1);
       }
       printf("Accepted new client socket. Client name: %s\n", name_client);

      //if connection was succesfully established,
      //server recieves first and then enters the call function
      struct client client;
      // client.buf = name_client;
      strcpy(client.buf,name_client);


      client.name_len=strlen(name_client);
      client.buf[strlen(name_client)] = ':';
      client.buf[256]='\0';
      client.sockfd=confd;

      ret = chat_server(confd,&open,&client);
      if(open==1){
        free(name_client);
      }

       //server recieves first and then sends


    // step (5) connection estabilished!  USE confd to have
    //          a dedicatedchat session with this client
    // I suggest passing confd to a function that handles this chat session
    // (use good modular design)
    // Also add common messaging functions to cs87talk.[ch]
    // (functionality shared by both the client and server)

    // note: make sure to close the socket returned by accept
    //       after the chat session ends (or on some error in the session)
  }

  close(listenfd);
}
/*********************************************/
