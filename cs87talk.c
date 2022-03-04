/*
 * Swarthmore College, CS 87
 * Copyright (c) 2020 Swarthmore College Computer Science Department,
 * Swarthmore PA, Professor Tia Newhall
 */
/*
 * Messaging (and other) functions shared by both the client
 * and the server
 *
 * (your names, date)
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "cs87talk.h"




int sender_Hello(int sockfd, size_t len, char* name){




  //send the HELLO_MSG TAG
  unsigned char buf;
  buf = HELLO_MSG;

  int ret = send(sockfd,&buf, 1, 0);
  if(ret <0) {
    printf("sending failed");
    return -1;
  }

  //send len
  buf = len;
  ret= send(sockfd,&buf, 1, 0);
  if(ret <0) {
    printf("len failed");
    return -1;
  }

  //send name
  char* buf2 = malloc(sizeof(char)*len);
  strcpy(buf2,name);

  ret= send(sockfd,buf2, len, 0);
  if(ret <0) {
    printf("send name not working");
    return -1;
  }
  free(buf2);

  ret= recv(sockfd,&buf,1,0);
  if(ret < 0) {
    printf("receiving response broke\n");
    perror("current error value");
    return -1;
  }
  if((int)buf==(int)HELLO_OK){
    return 0;

  }else if((int)buf==(int)HELLO_ERROR){
    return -1;
  }
  return -1;//succesfully sent
}


int receive_Hello(int sockfd, int* open,char* name){
  unsigned char buf[1];
  int status = HELLO_OK;

  //receive the 1 bit response
  int ret= recv(sockfd,buf,1,0);
  if(ret<0){
    perror("problem receiving hello");
    status = HELLO_ERROR;
  }
  //if hello message received and we can take on a client
  if((int)(buf[0]) == (int)HELLO_MSG && *open == 1){

    open = false;
    ret= recv(sockfd,buf,1,0);
    if(ret<0){
      perror("problem receiving length");
      status = HELLO_ERROR;
    }

    int len = (int)buf[0];

    int ret= recv(sockfd,name,len,0);
    if(ret<0){
          perror("problem receiving name");
          status = HELLO_ERROR;
        }
    name[len]='\0';

    ret= send(sockfd,&status, 1, 0);
    if(ret <0) {
          perror("problem sending the final message");
          return -1;
        }
    open =0;
    return 1;

  }
  else{
    status = HELLO_ERROR;
    ret= send(sockfd,&status, 1, 0);
    if(ret <0) {
      perror("Connection not possible");
      return -1;
    }
    return -1;

}
}


int sender(int sockfd, char* buf, size_t len, int flags){
    int done = 0;
    int start = 0;
    unsigned char tag[1];
    tag[0] = MSG_DATA;

    int ret = send(sockfd, tag, 1, MSG_NOSIGNAL);

    if(ret<0){
      if(errno==EPIPE){
        perror("Connection was unexpectedly killed and send is not working");
        return 3;
      }
      perror("Not sent properly");
      return -1;
    }

    unsigned char length[1];

    length[0] = (int)len;
    ret = send(sockfd, length, 1, MSG_NOSIGNAL);

    if(ret<0){
      if(errno==EPIPE){
        perror("Connection was unexpectedly killed and send is not working");
        return 3;
      }
      perror("Not sent properly");
      return -1;
    }

    while(done !=1){
      ret = send(sockfd,buf+start, len, MSG_NOSIGNAL);
      start += ret;
      if(ret==-1){
        if(errno==EPIPE){
          perror("Connection was unexpectedly killed and send is not working");
          return 3;
        }
        perror("problem sending");
        return -1;
      }
      else if(start == len){
        buf[len] = '\0';
        return 1;
      }

    }
  return 1;
}

// unsigned char

int receiver(int sockfd, char* buf, int flags, int name_len){
    int done = 0;
    int start = 0;
    unsigned char tag[1];
    unsigned char len[1];

    int ret1= recv(sockfd,tag,1,0);
    if (ret1 ==0){
      perror("Socket was closed");
      return 3;
    }
    if(ret1 < 0){
      perror("something is wrong");
      exit(1);
    }

    int ret2 = recv(sockfd,len,1,0);
    int length = (int)(len[0]);

    if (ret2 ==0){
      perror("Socket was closed");
      return 3;
    }


    if(ret2 < 0){

      perror("something is wrong");
      return -1;
    }

    if(length>256){
      length=256-name_len;
    }

    if((int)tag[0] == (int)QUIT){
      buf[0] = QUIT;
      buf[1] = '\0';
      return 1;

    }else if((int)(tag[0]) == (int)MSG_DATA){

      while(done !=1){
        int ret = recv(sockfd,buf+start, length, 0);
        if (ret2 ==0){
          printf("Socket was closed\n");
          perror("Socket was closed");
          return 3;
        }
        if(ret==-1){

          perror("problem receiving");
          return 1;
        }else if(ret == length){
          buf[length] = '\0';

          return 1;
        }else{
          done += ret;
          start+=ret;
        }
      }
    }else{
      printf("Else of reciever\n");
      return 1;
    }
return 1;
}



int chat_client(int sockfd, char* name){
    int running =1;

    while (running !=0){
      char* message = readline("$:  ");
      if(message==NULL){
        perror("Error in readline");
        return -1;
      }
      char* quit = "goodbye";

      if(strcmp(message,quit)==0){
        char q = QUIT;
        int ret = sender(sockfd, &q, 1, 0);
        running =0;
        return -1;
      }
      else{
        int ret = sender(sockfd, message, strlen(message), 0);
        if(ret < 0) {
          perror("Not recieved properly");
          return -1;
          }
        ret = receiver(sockfd, message, 0, 0);

        if(ret < 0) {
          perror("Not recieved properly");
          return 1;
          }
          printf("%s\n",message);

      }
    }
return 1;
}

int chat_server(int sockfd, int* open, struct client* client){
    int running =1;
    char* message=client->buf+client->name_len+1;

    while (running !=0){
      int ret = receiver(sockfd, message, 0,client->name_len);
      if(ret < 0) {
          perror("Server not recieving properly in chat");
          exit(1);
      }
      //if return value indicates the socket being shut off by client
      if(ret==3){
        //set open = 1
        *open = 1;
        return 1;
      }

      printf("%s\n",client->buf);

      //if the client says to quit, then quit
      if(*message==QUIT){
        //close the loop and return 1
        *open = 1;
        printf("Client has left the chat\n");
        return 1;
        }

      //if the client is not quitting, then ask for a message
      char * message = readline("$:");
      if(message==NULL){
        perror("Error in readline");
        exit(1);
      }


      ret = sender(sockfd, message, strlen(message), 0);
      if(ret < 0) {
          perror("Not recieved properly");
          return -1;
          }
      if(ret==3){
            //set open = 1
            *open = 1;
            return 1;
          }

    }
  return 1;
}




/**************************************************************/
