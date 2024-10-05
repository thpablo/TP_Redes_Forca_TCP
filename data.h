#ifndef DATA_H
#define DATA_H
#include<pthread.h>

enum{CHAT, GUESS};
enum{RIGHT, WRONG, WINNER, LOSER, NOTHING};
enum{INGAME, LOST, WON};

//dados do cliente ao server
typedef struct{
    int type;//0 = chat, 1 =guess
    char buffer[1024];
}ClientData;

//dados enviados do server aos clientes, adicione o que for necessário
typedef struct{
    int wrongGuesses;
    char chatBuffer[1024];
    char shownWord[100];
    char wrongLetters[100]; //string com todas as palavras erradas até o momento
    int isAMessageFromServer; // 0 = Printa palavra escondida, 1 = Printa buffer do chat
    int flag; //RIGHT, WRONG, WINNER, LOSER, NOTHING
    int yourTurn; // BOOL

}ServerData;

typedef struct str_thdata{
  int sock;
  pthread_t thread;
} thdata;


#endif