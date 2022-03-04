/*
 * Swarthmore College, CS 87
 * Copyright (c) 2020 Swarthmore College Computer Science Department,
 * Swarthmore PA, Professor Tia Newhall
 */

/* cs87talk.h
 *
 * Definitions and prototypes shared by client and server side
 * of the cs87talk interface (similar to unix talk)
 *
 * (your names, date)
 */

#ifndef _CS87TALK_H_
#define _CS87TALK_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <errno.h>

#include<readline/readline.h>
#include<readline/history.h>



// ***************** START: DO NOT CHANGE THESE DEFINITOINS
// The cs87talk server listen port number:
#define CS87_TALKSERVER_PORTNUM	1888

// Message Tag definitions:
#define HELLO_MSG               1
#define MSG_DATA                2
#define QUIT                    3
#define HELLO_OK                4
#define HELLO_ERROR             5

#define SERVER_COMP             1
#define CLIENT_COMP             2



// max size of a message associated with MSG_DATA
#define BUFMAX    256
// max name prefix for server-side message.  displayed as:
//   name:message
#define NAMEMAX   16


struct client {
    char buf[BUFMAX];
    int sockfd;
    int name_len;

} client;



// type to store/send/recve message tags and message sizes
// use tsize_t to declare variables storing these values:
//   tsize_t  len, tag;
// NOTE: can store up to 255 (related to BUFMAX size)
typedef unsigned char tsize_t;
// ***************** END: DO NOT CHANGE THESE DEFINITOINS

// Add any other definitions, function prototypes, etc. here:

/*
Inputs: takes in a socket id, a buffer, message buffer, length of the buffer,
Outputs:
Function:
*/




int sender_Hello(int sockfd, size_t len, char* name);
int receive_Hello(int sockfd, int* open, char* name);

int chat_client(int sockfd, char* name);
int chat_server(int sockfd,int* open,struct client* client);




int sender(int sockfd, char* buf, size_t len, int flags);
int receiver(int sockfd, char* buf, int flags, int name_len);



#endif
