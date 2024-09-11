#ifndef DATA_H
#define DATA_H

typedef enum{CHAT, GUESS}ClientDataType;
typedef enum{RIGHT, WRONG, WINNER, LOSER}FlagType;

//dados do cliente ao server
typedef struct{
    ClientDataType type;//0 = chat, 1 =guess
    char buffer[1024];
}ClientData;

//dados enviados do server aos clientes, adicione o que for necessário
typedef struct{
    char chatBuffer[1024];
    char shownWord[100];
    int isAMessageFromServer; // 0 = Printa palavra escondida, 1 = Printa buffer do chat
    FlagType flag;
    // char wrongLetters[26];

}ServerData;



#endif