
/****************** SERVER CODE ****************/
#include "chat.h"
#include "data.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <pthread.h>    /* POSIX Threads */ 
#include <stdlib.h>

#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <arpa/inet.h>    // htons(), inet_addr()
#include <sys/types.h>    // AF_INET, SOCK_STREAM

#define NUM_THREADS 30

int socketsThreadsIdsChat[NUM_THREADS];

pthread_mutex_t mutexChat;


void* conexao( void *param){

  thdata *data;            
  data = (thdata *) param;  /* type cast to a pointer to thdata */

  char buffer[1024], *result;
  int i;
  do {

    printf("Esperando Mensagem do cliente...\n");
    recv(data->sock, buffer, sizeof(buffer), 0);

    printf("Mensagem recebida do cliente = %s\n", buffer);

    char substring[5] = "quit";
    result = strstr(buffer, substring);
    if (result == NULL ){
      printf("enviando mensagem para os demais clientes, exlcuindo o remetente....\n");

      pthread_mutex_lock(&mutexChat);
      for (i = 0; i < NUM_THREADS; i++) {
        if (socketsThreadsIdsChat[i] != -1 && socketsThreadsIdsChat[i] != data->sock){
          send(socketsThreadsIdsChat[i],buffer,sizeof(buffer),0);
        }
      }

      pthread_mutex_unlock(&mutexChat);

    }

  }while (result == NULL);

  pthread_mutex_lock(&mutexChat);
  socketsThreadsIdsChat[data->thread] = -1; 
  pthread_mutex_unlock(&mutexChat);

  printf("fechando conexao...\n");
  shutdown(data->sock, 2);
  pthread_exit(NULL);

}


void*  chatMain(void *param){
  int welcomeSocket, newSocket;

  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  pthread_t threads[NUM_THREADS];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  thdata data[NUM_THREADS];

  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7892); // lembrar de alterar se necessário

  // lembrar de alterar se o servidor e cliente estiverem em máquinas diferentes. Nesse caso, colocar o IP da máquina que será servidora
  // o IP 127.0.0.1 só funciona se cliente e servidor estiverem na mesma máquina
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  pthread_mutex_init(&mutexChat, NULL);

  // informando que o socket irá ouvir até NUM_THREADS conexões
  if(listen(welcomeSocket,NUM_THREADS)==0)
    printf("Listening\n");
  else
    printf("Error\n");


  int i;

  // inicializando o vetor que conterá as referências para as threads. -1 indica que não existe thread associada.
 
  for (i = 0; i < NUM_THREADS; i++){
    socketsThreadsIdsChat[i] = -1;
  }

  // esperar no máximo NUM_THREADS conexões
  i = 0;
  while (i < NUM_THREADS) {

    printf("esperando conexao do cliente.... \n");
    newSocket = accept(welcomeSocket, (struct sockaddr *) NULL, NULL);

    pthread_mutex_lock(&mutexChat);
    socketsThreadsIdsChat[i] = newSocket; // newSocket é um ID gerado pelo sistema operacional
    data[i].thread = i;
    data[i].sock = newSocket;
    pthread_create (&threads[i], &attr, &conexao, (void *) &data[i]);
    printf ("cliente conectou.\n");
    i++;
    pthread_mutex_unlock(&mutexChat);
  }


  printf("Abriu todas as threads. Esperando a thread terminar para fechar o servidor.\n");

  for (i = 0; i < NUM_THREADS; i++){
    pthread_join(threads[i], NULL);
  }


  /* Clean up and exit */
   pthread_attr_destroy(&attr);
   pthread_mutex_destroy(&mutexChat);

  return nullptr;
}