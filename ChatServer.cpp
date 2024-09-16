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

#include <iostream>
#include <fstream>

#include <algorithm> // Para std::transform
#include <cctype>	 // Para std::tolower

#include <cstring> // Para std::strcpy
#include "data.h"

using namespace std;

#define NUM_THREADS 30

int socketsThreadsIds[NUM_THREADS]; // Vetor de sockets
int connectedPlayers = 0;			// Contador de jogadores conectados

bool difficultyChosen = false;	// Verifica se ja escolheu dificuldade
int playerChooseDifficulty = 0; // Jogador que escolhe a dificuldade
bool inACurrentGame = false;	// Verifica se existe um jogo em andamento

pthread_mutex_t mutex;										// Mutex para controle de acesso a variáveis compartilhadas
pthread_cond_t cond_two_players = PTHREAD_COND_INITIALIZER; // Mutex para espera de 2 players
pthread_cond_t cond_has_a_game = PTHREAD_COND_INITIALIZER;	// Mutex para espera de 2 players

typedef struct str_thdata
{
	int thread_no;
	int sock;
} thdata;

// Classe para o chat e contar tentativas restantes
class Player
{
public:
	thdata dataPlayer;
	string name;
	int contError; // Somador com numero de erros
};

thdata playersData[NUM_THREADS]; // Vetor de dados dos jogadores

// Classe para guardar as palavras
class Words
{
	vector<string> words;

public:
	void loadFromFile(const std::string &filename)
	{
		std::ifstream file(filename);
		if (!file)
		{
			std::cerr << "Erro ao abrir o arquivo " << filename << std::endl;
			return;
		}

		std::string word;
		while (file >> word)
		{
			words.push_back(word);
		}

		file.close();
	}

	void printWords() const
	{
		cout << "Palavras disponiveis: ";
		for (const auto &word : words)
		{
			cout << word << " ";
		}
		cout << endl;
	}

	string randomWord()
	{
		int n = rand() % words.size();
		return words[n];
	}
};

// Escolhe palavra de acordo com a dificuldade
string chooseWord(string difficulty)
{
	Words word;
	// MUDAR IFs para switch case
	if (difficulty == "facil")
	{
		word.loadFromFile("./palavras/facil.txt");
		word.printWords();
		return word.randomWord();
	}
	else if (difficulty == "medio")
	{
		word.loadFromFile("./palavras/medio.txt");
		word.printWords();
		return word.randomWord();
	}

	else if (difficulty == "dificil")
	{
		word.loadFromFile("./palavras/dificil.txt");
		word.printWords();
		return word.randomWord();
	}

	return "";
}
class Hangman
{
private:
	string wordSecret;
	string wordShown;
	int maxError;
	int contError;

public:
	// Construtor público para permitir múltiplas instâncias
	Hangman()
	{
		contError = 0; // Inicializando o contador de erros
	}

	void createGame(string difficulty)
	{
		wordSecret = chooseWord(difficulty); // Escolhe palavra de acordo com a dificuldade
		cout << "Palavra secreta: " << wordSecret << endl;
		hideWord();						  // Esconde palavra com "_"
		maxError = wordSecret.size() + 2; // Erros máximo = tamanho da palavra + 2
	}

	string getWordShown()
	{
		return wordShown;
	}

	// Constroi wordShown escondendo palavra
	void hideWord()
	{
		string w(wordSecret.size(), '_');
		wordShown = w;
	}

	// Desenha partes da forca (com número máximo de erros)
	void drawPartsOfHangman()
	{
		contError++;
		/*
			Lógica para desenho
		*/
		std::cout << "Desenhou parte da forca. Erros: " << contError << "/" << maxError << std::endl;
	}

	// Verifica se ganhou
	bool win()
	{
		return (wordSecret == wordShown);
	}

	// Joga com a palavra ou letra e decide se ganhou ou perdeu
	int play(string word)
	{
		if (check(word))  // Se houve correspondência a palavra ou letra
			return win(); // Retorna se ganhou
		else
		{
			drawPartsOfHangman(); // Desenha partes do corpo
			// Verifica se houve erros máximos
			if (contError == maxError)
				return -1; // Perde, fim de jogo
		}
		return false; // Não ganha, mas continua jogando
	}

	// Verifica se contém letra ou é a palavra
	bool check(string s)
	{
		bool found = false;

		if (s.size() == 1)
		{ // Verifica letra
			for (long unsigned int i = 0; i < wordSecret.size(); i++)
			{
				if (s[0] == wordSecret[i])
				{
					wordShown[i] = s[0];
					found = true; // Tem letra na palavra
				}
			}
			return found;
		}
		else // Verifica palavra
		{
			if (s == wordSecret)
			{
				wordShown = wordSecret;
				return true; // Palavra correta
			}
		}
		return false; // Sem correspondência
	}
};

// Converte string para chatBuffer na struct ServerData
ServerData convertToChatBuffer(string msg)
{
	ServerData sendData;
	sendData.isAMessageFromServer = 1;							 // Inicializa a flag indicando que é uma mensagem do servidor
	memset(sendData.chatBuffer, 0, sizeof(sendData.chatBuffer)); // Limpa o buffer antes de copiar a mensagem
	strcat(sendData.chatBuffer, msg.c_str());					 // Copia a mensagem para o buffer
	// Remove o caractere de nova linha (\n), se presente
	char *pos = strchr(sendData.chatBuffer, '\n');
	if (pos)
	{
		*pos = '\0'; // Substitui '\n' por '\0'
	}
	return sendData;
}

// Converte gameStatus para string
string resGame(int gameStatus)
{
	if (gameStatus == -1)
		return "Perdeu";
	else if (gameStatus == 1)
		return "Ganhou";
	else
		return "";
}

// Converte Char para String retirando o \n
string convertCharToString(char *c)
{
	string str = c;
	if (!str.empty() && str.back() == '\n')
	{
		str.pop_back(); // Remove o último caractere
	}
	return str;
}

void inGame(Hangman &game, thdata &player1, thdata &player2)
{
	inACurrentGame = true; // Indica que existe um jogo em andamento
	thdata currentPlayer;  // Dados do jogador atual
	thdata anotherPlayer;  // Dados do outro jogador

	int whoIsPlaying = rand() % 2; // Controle do jogador atual, primeiro a jogar é randomizado
	int gameStatus = 0;			   // Status do jogo
	ServerData sendData;		   // Dados envio servidor -> cliente
	sendData.isAMessageFromServer = 0;
	ClientData cData; // Dados recebimento cliente -> servidor

	while (gameStatus != -1 && gameStatus != 1)
	{
		// decide quem é o jogador atual
		currentPlayer = (whoIsPlaying == 0) ? player1 : player2;
		anotherPlayer = (whoIsPlaying == 0) ? player2 : player1;

		cout << "Jogador Atual: " << currentPlayer.thread_no << endl;

		// Mostrar palavra atualizada para todos
		string wordShown = game.getWordShown(); // Captura mensagem escondida
		sendData.flag = RIGHT;
		sendData.isAMessageFromServer = 0; // Condição para enviar a palavra escondida
		strcpy(sendData.shownWord, wordShown.c_str());

		// Envia a palavra escondida para os dois jogadores
		send(player1.sock, &sendData, sizeof(ServerData), 0);
		send(player2.sock, &sendData, sizeof(ServerData), 0);

		// Envia mensagem para outro jogador aguardar sua vez
		sendData = convertToChatBuffer("Aguarde sua vez");
		send(anotherPlayer.sock, &sendData, sizeof(ServerData), 0);

		sendData = convertToChatBuffer("Sua vez");
		send(currentPlayer.sock, &sendData, sizeof(ServerData), 0);
		recv(currentPlayer.sock, &cData, sizeof(ClientData), 0);
		cout << "Mensagem recebida do cliente = " << cData.buffer << endl;

		// Converte o input recebido para string
		string input = convertCharToString(cData.buffer);

		// Joga com a palavra ou letra e decide se ganhou ou perdeu
		gameStatus = game.play(input);
		cout << "Game Status: " << gameStatus << endl;
		// Verifica vencedor ou perdedor
		if (gameStatus == -1 || gameStatus == 1)
		{
			cout << "Fim de jogo\n"
				 << endl;
			cout << "Jogador " << (currentPlayer.thread_no + 1) << " " << resGame(gameStatus) << endl;
			sendData.flag = (gameStatus == 1) ? WINNER : LOSER; // Define o flag de acordo com o resultado
			strcpy(sendData.shownWord, wordShown.c_str());
			for (int i = 0; i < NUM_THREADS; i++)
			{
				if (socketsThreadsIds[i] != -1)
				{
					send(socketsThreadsIds[i], &sendData, sizeof(ServerData), 0);
				}
			}
			pthread_mutex_unlock(&mutex);
			break; // Termina o loop se o jogo acabou
		}

		whoIsPlaying = (whoIsPlaying + 1) % 2; // Alterna o jogador
		cout << "Alternando o jogador: " << whoIsPlaying << endl;
	}

	cout << "Status: " << resGame(gameStatus) << endl;
	cout << "Fim de jogo\n"
		 << endl;
	inACurrentGame = false;					  // Indica que não existe um jogo em andamento
	pthread_cond_broadcast(&cond_has_a_game); // Acorda todas as threads para que possam jogar
}

// Lobby espera dois players se conectarem
void *lobby(void *param)
{
	thdata *data;
	data = (thdata *)param; /* type cast to a pointer to thdata */
	ServerData sendData;
	Hangman game;

	pthread_mutex_lock(&mutex);
	playersData[data->thread_no] = *(data);
	pthread_mutex_unlock(&mutex);

	// Envia número da thread para o jogador
	sendData = convertToChatBuffer("Você é o jogador " + to_string(data->thread_no + 1));
	send(data->sock, &sendData, sizeof(ServerData), 0);

	// Espera dois jogadores se conectarem
	if ((connectedPlayers % 2) == 1) // Se tiver quantidade impar de jogadores, espera a dupla
	{								 // Espera dois jogadores se conectarem
		cout << "Esperando outro jogador se conectar" << endl;
		sendData = convertToChatBuffer("Aguardando outro jogador conectar");
		send(data->sock, &sendData, sizeof(ServerData), 0); // Avisa cliente que esta esperando outro jogador
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond_two_players, &mutex); // Espera outro jogador se conectar
		pthread_mutex_unlock(&mutex);
	}

	/*
	// Verifica se existe jogo, se sim, espera no lobby
	if (!inACurrentGame)
	{
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond_has_a_game, &mutex); // Espera um jogo ser criado
		pthread_mutex_unlock(&mutex);
	}
	*/

	cout << "Jogador " << (data->thread_no + 1) << " conectado. Iniciando jogo..." << endl;

	// Randomiza a escolha do jogador que escolhe a dificuldade
	pthread_mutex_lock(&mutex);
	// Verifica se ja escolheu dificuldade, se não, randomiza
	if (!difficultyChosen)
	{
		playerChooseDifficulty = rand() % 2;
		difficultyChosen = true;
	}
	pthread_mutex_unlock(&mutex);

	// Escolhe a dificuldade
	pthread_mutex_lock(&mutex);
	if ((data->thread_no % 2) != playerChooseDifficulty)
	{
		pthread_cond_wait(&cond_two_players, &mutex); // Espera o outro jogador escolher a dificuldade
	}

	else if ((data->thread_no % 2) == playerChooseDifficulty)
	{
		// Envia mensagem para o jogador 0 escolher a dificuldade
		cout << "Enviando mensagem para jogador escolher dificuldade" << endl;
		sendData = convertToChatBuffer("Escolha a dificuldade para o jogo\n");
		send(data->sock, &sendData, sizeof(ServerData), 0);

		// Recebe a dificuldade escolhida pelo jogador 0
		ClientData cData;
		recv(data->sock, &cData, sizeof(ClientData), 0);
		cout << convertCharToString(cData.buffer);
		string difficulty = convertCharToString(cData.buffer);
		cout << "Dificuldade escolhida: " << difficulty << endl;

		// Cria jogo com a dificuldade escolhida
		game.createGame(difficulty);			   // Cria jogo com dificuldade escolhida
		pthread_cond_broadcast(&cond_two_players); // Acorda a thread do outro jogador para que possa jogar

		// Passa dados dos dois players para jogo
		inGame(game, playersData[connectedPlayers - 2], playersData[connectedPlayers - 1]); // Inicia o jogo
	}

	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

int main()
{
	srand(time(NULL));
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
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
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

		connectedPlayers++;
		printf("cliente conectou.\n");
		// Quando o segundo jogador conectar, todos podem prosseguir
		if ((connectedPlayers % 2) == 0)
		{
			pthread_cond_broadcast(&cond_two_players); // Acorda todas as threads
		}

		pthread_create(&threads[i], &attr, &lobby, (void *)&data[i]);
		i++;
		pthread_mutex_unlock(&mutex);
	}

	printf("Abriu todas as threads. Esperando a thread terminar para fechar o servidor.\n");

	for (i = 0; i < NUM_THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}

	/* Clean up and exit */
	pthread_cond_destroy(&cond_two_players);
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&mutex);

	return 0;
}