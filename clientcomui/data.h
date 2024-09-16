#ifndef DATA_H
#define DATA_H


#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>    /* POSIX Threads */ 
#include <stdlib.h>

#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <arpa/inet.h>    // htons(), inet_addr()
#include <sys/types.h>    // AF_INET, SOCK_STREAM
#include "data.h"
#include <iostream>
using namespace std;

// typedef enum{CHAT, GAME}ClientDataType;
// typedef enum{RIGHT, WRONG, WINNER, LOSER}FlagType;

// //dados do cliente ao server
// typedef struct clientdata{
//     ClientDataType type;//0 = chat, 1 =guess
//     char buffer[1024];
// }ClientData;

// //dados enviados do server aos clientes, adicione o que for necessário
// typedef struct serverdata{
//     char chatBuffer[1024];
//     char shownWord[100];
//     int isAMessageFromServer; // 0 = Printa palavra escondida, 1 = Printa buffer do chat
//     FlagType flag;
//     // char wrongLetters[26];

// }ServerData;

// typedef struct str_thdata{
//   int sock;
//   pthread_t thread;
// } thdata;



int connect_socket(int __fd, const sockaddr *__addr, socklen_t __len){//evitar erro com o connect do qt
  return connect(__fd, __addr, __len);
}

#endif