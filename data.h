#ifndef DATA_H
#define DATA_H
#include<pthread.h>

enum{CHAT, GUESS};
enum{RIGHT, WRONG, WINNER, LOSER, INPUT};
enum{INGAME, LOST, WON};

//dados do cliente ao server
typedef struct{
    int type;//0 = chat, 1 =guess
    char buffer[1024];
}ClientData;

//dados enviados do server aos clientes, adicione o que for necessário
typedef struct{
    char chatBuffer[1024];
    char shownWord[100];
    int isAMessageFromServer; // 0 = Printa palavra escondida, 1 = Printa buffer do chat
    int flag;
    char wrongLetters[100];
    int yourTurn;

}ServerData;

typedef struct str_thdata{
  int sock;
  pthread_t thread;
} thdata;


#endif