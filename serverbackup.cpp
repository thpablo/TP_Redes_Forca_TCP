// gcc ChatServer.c -o ChatServer -lpthread

/****************** SERVER CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h> // c
#include <string>	// cpp
#include <time.h>
#include <pthread.h> /* POSIX Threads */
#include <stdlib.h>

#include <fcntl.h>	   // for open
#include <unistd.h>	   // for close
#include <arpa/inet.h> // htons(), inet_addr()
#include <sys/types.h> // AF_INET, SOCK_STREAM

#include <chrono> // wait na funcao
#include <thread>
#include <vector> 

using namespace std;

#define NUM_THREADS 30
#define NUM_PLAYERS 2

int socketsThreadsIds[NUM_THREADS];
bool CURRENT_GAME = false;

pthread_mutex_t mutex;

typedef struct str_thdata
{
	int thread_no;
	int sock;
} thdata;

// Classe para guardar as palavras
class Words{
	vector<string> words;
};


void *conexao(void *param)
{
	thdata *data;
	data = (thdata *)param; /* type cast to a pointer to thdata */

	char buffer[1024], *result;
	int i;
	do
	{
		printf("Esperando Mensagem do cliente...\n");
		recv(data->sock, buffer, sizeof(buffer), 0);

		printf("Mensagem recebida do cliente = %s\n", buffer);

		char substring[5] = "quit";
		result = strstr(buffer, substring);
		if (result == NULL)
		{
			printf("enviando mensagem para os demais clientes, exlcuindo o remetente....\n");

			pthread_mutex_lock(&mutex);
			for (i = 0; i < NUM_THREADS; i++)
			{
				if (socketsThreadsIds[i] != -1 && socketsThreadsIds[i] != data->sock)
				{
					send(socketsThreadsIds[i], buffer, sizeof(buffer), 0);
				}
			}

			pthread_mutex_unlock(&mutex);
		}

	} while (result == NULL);

	pthread_mutex_lock(&mutex);
	socketsThreadsIds[data->thread_no] = -1;
	pthread_mutex_unlock(&mutex);

	printf("fechando conexao...\n");
	shutdown(data->sock, 2);
	pthread_exit(NULL);

	return NULL;
}

/*
void *lobby(void *param){
		// Verifica se numero do player é menor que 2 (quantidade maxima de players)
		if (data[i].thread_no < NUM_PLAYERS)
		{
			CURRENT_GAME = true; // Jogo corrente
			pthread_create(&threads[i], &attr, &conexao, (void *)&data[i]);
		}
		else{ // Se nao, espera na fila
			pthread_create(&threads[i], &attr, &waitOnQueue, (void *)&data[i]);
		}
}

void *waitOnQueue(void *param)
{
	thdata *data = (thdata *)param; // Dados da thread

	string waitToGame = "AGUARDE FINALIZAR JOGO\n";

	do
	{
		// mensagem a cada 10 segundos para esperar
		cout << waitToGame << endl;
		send(socketsThreadsIds[data->thread_no], &waitToGame, sizeof(waitToGame), 0);
		this_thread::sleep_for(std::chrono::seconds(10));
	} while (CURRENT_GAME); // Enquanto existir um jogo corrente

	return NULL;
}
*/

int main()
{
	int welcomeSocket, newSocket;

	struct sockaddr_in serverAddr;
	socklen_t addr_size;
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	// Cria 30 structs de dados (  int thread_no; int sock;)
	thdata data[NUM_THREADS];

	// Cria socket
	welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7891); // lembrar de alterar se necessário

	// lembrar de alterar se o servidor e cliente estiverem em máquinas diferentes. Nesse caso, colocar o IP da máquina que será servidora
	// o IP 127.0.0.1 só funciona se cliente e servidor estiverem na mesma máquina
	serverAddr.sin_addr.s_addr = inet_addr("192.168.1.5");
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	bind(welcomeSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

	pthread_mutex_init(&mutex, NULL);

	// informando que o socket irá ouvir até NUM_THREADS conexões
	// Limitar quantidade máxima de jogares em NUM_THREADS (2)
	if (listen(welcomeSocket, NUM_THREADS) == 0)
		printf("Listening\n");
	else
		printf("Error\n");

	int i;

	// inicializando o vetor que conterá as referências para as threads. -1 indica que não existe thread associada.

	for (i = 0; i < NUM_THREADS; i++)
	{
		socketsThreadsIds[i] = -1;
	}

	// esperar no máximo NUM_THREADS conexões
	i = 0;
	while (i < NUM_THREADS)
	{

		printf("esperando conexao do cliente.... \n");
		newSocket = accept(welcomeSocket, (struct sockaddr *)NULL, NULL);

		pthread_mutex_lock(&mutex);
		socketsThreadsIds[i] = newSocket; // newSocket é um ID gerado pelo sistema operacional
		data[i].thread_no = i;
		data[i].sock = newSocket;

		printf("cliente conectou.\n");
    	pthread_create (&threads[i], &attr, &conexao, (void *) &data[i]);

		pthread_mutex_unlock(&mutex);
	}

	printf("Abriu todas as threads. Esperando a thread terminar para fechar o servidor.\n");

	for (i = 0; i < NUM_THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}

	/* Clean up and exit */
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&mutex);

	return 0;
}
