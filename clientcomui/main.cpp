#include "mainwindow.h"

#include <QApplication>


// #include <stdio.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <string.h>
// #include <pthread.h>    /* POSIX Threads */ 
// #include <stdlib.h>

// #include <fcntl.h> // for open
// #include <unistd.h> // for close
// #include <arpa/inet.h>    // htons(), inet_addr()
// #include <sys/types.h>    // AF_INET, SOCK_STREAM
// #include <iostream>
// using namespace std;



// typedef enum{CHAT, GUESS}ClientDataType;
// typedef enum{RIGHT, WRONG, WINNER, LOSER}FlagType;

// //dados do cliente ao server
// typedef struct{
//     ClientDataType type;//0 = chat, 1 =guess
//     char buffer[1024];
// }ClientData;

// //dados enviados do server aos clientes, adicione o que for necessário
// typedef struct{
//     char chatBuffer[1024];
//     char shownWord[100];
//     int isAMessageFromServer; // 0 = Printa palavra escondida, 1 = Printa buffer do chat
//     FlagType flag;
//     // char wrongLetters[26];

// }ServerData;





// bool end_flag = 0; //quando passa a ser 1, todas as threads encerram
// int chatType = -1;

// typedef struct str_thdata{
//   int sock;
//   pthread_t thread;
// } thdata;


// void* threadSendChat(void *param){
//   thdata *data;
//   data = (thdata *)param;
//   ClientData cData;
//   char msg[1024];

//   while(!end_flag){
//     printf("Digite alguma coisa (para sair, digite quit): \n");
//     memset(cData.buffer, '\0', sizeof(cData.buffer)); // resetar o buffer
//     fgets(cData.buffer, 1024, stdin);
//     //strcpy(buffer, nome);
//     //strcat(buffer, "-> ");
//     //strcat(cData.buffer, msg);
    
//     // Remove o caractere de nova linha (\n), se presente
//     char* pos = strchr(cData.buffer, '\n');
//     if (pos) {
//         *pos = '\0'; // Substitui '\n' por '\0'
//     }

//     send(data->sock,&cData,sizeof(ClientData),0);

//   }
//   return NULL;
// }

// void* threadSendGameGuess(void *param){
//   thdata *data;
//   data = (thdata *)param;



//   ClientData cData;
//   char msg[1024], nome[100];

//   while(!end_flag){
//     printf("Digite alguma coisa (para sair, digite quit): \n");
//     memset(cData.buffer, '\0', sizeof(cData.buffer)); // resetar o buffer
//     fgets(cData.buffer, 1024, stdin);
//     //strcpy(buffer, nome);
//     //strcat(buffer, "-> ");
//     //strcat(cData.buffer, msg);
    
//     // Remove o caractere de nova linha (\n), se presente
//     char* pos = strchr(cData.buffer, '\n');
//     if (pos) {
//         *pos = '\0'; // Substitui '\n' por '\0'
//     }

//     send(data->sock,&cData,sizeof(ClientData),0);

//   }
//   return NULL;
// }






// void* threadRecv(void *param){

//   thdata *data;
//   data = (thdata *) param;
 
//   char buffer[1024];
//   ServerData receivedData;

//   int s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
//   if (s != 0) {
//     printf("Erro.");
//     exit(0);
//   }

//   while (receivedData.flag != WINNER && receivedData.flag != LOSER){
//     recv(data->sock, &receivedData, sizeof(ServerData), 0);
//     if(receivedData.isAMessageFromServer == 1){
//       printf("%s\n",receivedData.chatBuffer);
//     }
//     else{
//       printf("Palavra: %s\n",receivedData.shownWord);
//     } 
//   }
//   printf("Fim de Jogo\n");

//   return NULL;

// }


// void* threadSend(void *param){
//   pthread_t threadChatId, threadGameGuessId;
//   thdata *data;
//   data = (thdata *) param;

//   char msg[1024], nome[100];// buffer[1024];

//   //printf("Digite seu nome: \n");

//   //fgets(nome, 100, stdin);
//   pthread_create (&threadChatId, NULL,  &threadSendChat, param);
//   pthread_create (&threadGameGuessId, NULL,  &threadRecv, param);

//   do {
    
//     scanf("%d", &chatType); //Tipo da mensagem enviada
//     sleep(1);
//   } while (strcmp(msg, "quit\n") != 0);

//   pthread_join(threadChatId, NULL);
//   pthread_join(threadGameGuessId, NULL);

//   printf("Fechando Conexao e encerrando o programa...\n"); 
//   pthread_cancel(data->thread);
//   sleep(5);
//   shutdown(data->sock, 2);
//   pthread_exit(NULL);
//   return NULL;

// }


// int main(int argc, char *argv[]){
//   int clientSocket;
//   struct sockaddr_in serverAddr;
//   socklen_t addr_size;
//   pthread_t threadSendId, threadRecvId;
//   thdata dataRecv, dataSend;
//   pthread_attr_t attr;

//   pthread_attr_init(&attr);
//   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

//   clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  
//   serverAddr.sin_family = AF_INET;
//   serverAddr.sin_port = htons(7891);
//   serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
//   memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  


//   addr_size = sizeof serverAddr;
//   connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));


//   printf("criando thread recv...\n");
//   dataRecv.sock = clientSocket;
//   pthread_create (&threadRecvId, &attr,  &threadRecv, (void *) &dataRecv);

//   printf("criando thread send...\n");
//   dataSend.sock = clientSocket;
//   dataSend.thread = threadRecvId;
//   pthread_create (&threadSendId, &attr,  &threadSend, (void *) &dataSend);
 
//   pthread_join(threadRecvId, NULL);
//   //pthread_cancel(threadSendId);
  

//   pthread_attr_destroy(&attr);

//   printf("Todas as threads terminaram. Fechando cliente.\n");


//   return 0;
// }




int main(int argc, char *argv[])
{   
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    printf("ssssssssss");
    
    return a.exec();
}
