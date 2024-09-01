#ifndef DATA_H
#define DATA_H

typedef enum{CHAT, GUESS}ClientDataType;
typedef enum{RIGHT, WRONG}SoundType;

typedef struct{
    ClientDataType type;//0 = chat, 1 =guess
    char buffer[1024];
}ClientData;

typedef struct{
    char buffer[1024];
    char shownWord[100];
    // char wrongLetters[26];
    // SoundType soundType;

}ServerData;



#endif