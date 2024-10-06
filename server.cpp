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
#include "chat.h"
#include <queue>

using namespace std;

#define NUM_THREADS 30
#define THREADS_TUPLE 3


/*** Teste fila */
queue<client_thdata *> fila;
/******** */

int socketsThreadsIds[NUM_THREADS]; // Vetor de sockets
int connectedPlayers = 0;			// Contador de jogadores conectados

bool difficultyChosen = false;	// Verifica se ja escolheu dificuldade
int playerChooseDifficulty = 0; // Jogador que escolhe a dificuldade
bool inACurrentGame = false;	// Verifica se existe um jogo em andamento

pthread_mutex_t mutex;										// Mutex para controle de acesso a variáveis compartilhadas
pthread_cond_t cond_two_players = PTHREAD_COND_INITIALIZER; // Mutex para espera de 2 players
pthread_cond_t cond_has_a_game = PTHREAD_COND_INITIALIZER;	// Mutex para espera de 2 players

// Classe para o chat e contar tentativas restantes
class Player
{
public:
	thdata dataPlayer;
	string name;
	int countError; // Somador com numero de erros
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
	int countError;
	int countWrongLetters; // quantidade de tentativas de erros de única letra (usado para navegar no vetor wrongLetters)

public:
	char wrongLetters[100];
	// Construtor público para permitir múltiplas instâncias
	Hangman()
	{
		countError = 0; // Inicializando o contador de erros
		countWrongLetters = 0;
	}

	void createGame(string difficulty)
	{
		wordSecret = chooseWord(difficulty); // Escolhe palavra de acordo com a dificuldade
		cout << "Palavra secreta: " << wordSecret << endl;
		hideWord();	  // Esconde palavra com "_"
		maxError = 6; // Erros máximo = tamanho da palavra + 2
	}

	int getCountError() { return countError; }

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
		countError++;
		/*
			Lógica para desenho
		*/
		std::cout << "Desenhou parte da forca. Erros: " << countError << "/" << maxError << std::endl;
	}

	// Verifica se ganhou
	int win()
	{
		return WON;
	}

	// Verifica correspondencia de letra ou palavra
	int play(string word)
	{
		if (check(word))									 // Se houve correspondência a palavra ou letra
			return wordSecret == wordShown ? WINNER : RIGHT; // Retorna se ganhou ou continua em jogo
		else
		{
			drawPartsOfHangman(); // Desenha partes do corpo
			// Verifica se houve erros máximos
			if (countError == maxError)
				return LOSER; // Perde, fim de jogo
		}
		return WRONG; // Não ganha, mas continua jogando
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
					wordShown[i] = s[0]; // Substituiu na palavra mostrada as letras corretas
					found = true;		 // Tem letra na palavra
				}
			}

			if (!found)
			{
				wrongLetters[countWrongLetters * 2] = s[0];
				wrongLetters[countWrongLetters * 2 + 1] = ' ';
				wrongLetters[countWrongLetters * 2 + 2] = '\0';
				countWrongLetters++;
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
string resGameToString(int gameStatus)
{
	if (gameStatus == LOST)
		return "Perdeu";
	else if (gameStatus == WON)
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
	int gameStatus = NOTHING;	   // Status do jogo: 0 = continua, 1 = ganhou, -1 = perdeu

	ServerData sendData; // Dados envio servidor -> cliente
	sendData.isAMessageFromServer = 0;
	ClientData cData; // Dados recebimento cliente -> servidor

	pthread_t chatThread;
	pthread_create(&chatThread, NULL, &chatMain, NULL);

	while (gameStatus != WINNER && gameStatus != LOSER)
	{
		// decide quem é o jogador atual
		currentPlayer = (whoIsPlaying == 0) ? player1 : player2;
		anotherPlayer = (whoIsPlaying == 0) ? player2 : player1;

		cout << "Jogador Atual: " << currentPlayer.thread + 1 << endl;

		// Mostrar palavra atualizada para todos
		string wordShown = game.getWordShown(); // Captura mensagem escondida
		sendData.flag = gameStatus;
		sendData.isAMessageFromServer = 0; // Condição para enviar a palavra escondida
		strcpy(sendData.shownWord, wordShown.c_str());
		strcpy(sendData.wrongLetters, game.wrongLetters);
		sendData.wrongGuesses = game.getCountError();

		sendData.yourTurn = 0;
		// Envia a palavra escondida para os dois jogadores
		send(anotherPlayer.sock, &sendData, sizeof(ServerData), 0);

		sendData.flag = NOTHING;
		sendData.yourTurn = 1;
		send(currentPlayer.sock, &sendData, sizeof(ServerData), 0);

		// Envia mensagem para outro jogador aguardar sua vez
		// sendData = convertToChatBuffer("Aguarde sua vez");

		recv(currentPlayer.sock, &cData, sizeof(ClientData), 0);
		cout << "Mensagem recebida do cliente = " << cData.buffer << endl;

		// Converte o input recebido para string
		string input = convertCharToString(cData.buffer);

		// Joga com a palavra ou letra e decide se ganhou ou perdeu
		gameStatus = game.play(input);
		cout << "Game Status: " << gameStatus << endl;
		// Verifica vencedor ou perdedor
		if (gameStatus == LOSER || gameStatus == WINNER)
		{
			string wordShown = game.getWordShown(); // Captura mensagem escondida
			sendData.isAMessageFromServer = 0;
			strcpy(sendData.shownWord, wordShown.c_str());
			strcpy(sendData.wrongLetters, game.wrongLetters);
			sendData.wrongGuesses = game.getCountError();

			sendData.flag = gameStatus; // Define o flag de acordo com o resultado
			send(currentPlayer.sock, &sendData, sizeof(ServerData), 0);

			sendData.flag = LOSER;
			send(anotherPlayer.sock, &sendData, sizeof(ServerData), 0);

			cout << "Fim de jogo\n"
				 << endl;
			cout << "Jogador " << (currentPlayer.thread + 1) << " " << resGameToString(gameStatus) << endl;

			pthread_mutex_unlock(&mutex);
			break; // Termina o loop se o jogo acabou
		}

		whoIsPlaying = (whoIsPlaying + 1) % 2; // Alterna o jogador
		cout << "Alternando o jogador: " << whoIsPlaying << endl;
	}

	cout << "Status: " << resGameToString(gameStatus) << endl;
	cout << "Fim de jogo\n"
		 << endl;
	pthread_join(chatThread, nullptr);
	inACurrentGame = false;					  // Indica que não existe um jogo em andamento
	pthread_cond_broadcast(&cond_has_a_game); // Acorda todas as threads para que possam jogar
}

/* Lobby espera dois players se conectarem
void *lobby(void *param)
{
	thdata *data;
	data = (thdata *)param; // type cast to a pointer to thdata
	ServerData sendData;
	Hangman game;

	pthread_mutex_lock(&mutex);
	playersData[data->thread] = *(data);
	pthread_mutex_unlock(&mutex);

	// Envia número da thread para o jogador
	sendData = convertToChatBuffer("Você é o jogador " + to_string(data->thread + 1));
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

	cout << "Jogador " << (data->thread + 1) << " conectado. Iniciando jogo..." << endl;

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
	if ((data->thread % 2) != playerChooseDifficulty)
	{
		pthread_cond_wait(&cond_two_players, &mutex); // Espera o outro jogador escolher a dificuldade
	}

	else if ((data->thread % 2) == playerChooseDifficulty)
	{
		// Envia mensagem para o jogador 0 escolher a dificuldade
		cout << "Enviando mensagem para jogador escolher dificuldade" << endl;
		// Recebe a dificuldade escolhida pelo jogador 0
		string difficulty = "";
		ClientData cData;
		while (difficulty != "facil" && difficulty != "medio" && difficulty != "dificil")
		{
			sendData = convertToChatBuffer("Server: Escolha a dificuldade para o jogo\n");
			send(data->sock, &sendData, sizeof(ServerData), 0);
			recv(data->sock, &cData, sizeof(ClientData), 0);
			printf("%s\n", cData.buffer);
			cout << convertCharToString(cData.buffer);
			difficulty = convertCharToString(cData.buffer);
		}

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
*/

// Struct para armazenar dupla de jogadores
struct PlayerPair
{
	thdata *player1;
	thdata *player2;
};

string decideDifficulty(thdata *playerWhosDecide, thdata *anotherPlayer)
{
	ServerData sendData;
	sendData = convertToChatBuffer("Server: Escolha a dificuldade para o jogo\n");
	send(playerWhosDecide->sock, &sendData, sizeof(ServerData), 0);
	sendData = convertToChatBuffer("Server: Aguarde o outro jogador escolher a dificuldade\n");
	send(anotherPlayer->sock, &sendData, sizeof(ServerData), 0);
	string difficulty = "";
	ClientData cData;
	while (difficulty != "facil" && difficulty != "medio" && difficulty != "dificil")
	{
		recv(playerWhosDecide->sock, &cData, sizeof(ClientData), 0);
		printf("%s\n", cData.buffer);
		cout << convertCharToString(cData.buffer);
		difficulty = convertCharToString(cData.buffer);
	}
	return difficulty;
}

void *lobby(void *param)
{
	PlayerPair *players = (PlayerPair *)param;

	thdata *player1 = players->player1;
	thdata *player2 = players->player2;
	delete players; // Libere a memória alocada para players
	ServerData sendData;
	Hangman game;
	string difficulty = "";

	// Randomiza quem Escolhe dificuldade do jogo
	int whoDecidesDifficulty = rand() % 2; // 0 = player1, 1 = player2
	if (whoDecidesDifficulty == 0)
	{
		difficulty = decideDifficulty(player1, player2);
	}
	else
	{
		difficulty = decideDifficulty(player2, player1);
	}
	cout << "Dificuldade escolhida: " << difficulty << endl;
	game.createGame(difficulty); // Cria jogo com dificuldade escolhida

	inGame(game, *player1, *player2); // Inicia o jogo
	return nullptr;
}

void *chatRoom(void *param)
{
	PlayerPair *players = (PlayerPair *)param;

	thdata *player1 = players->player1;
	thdata *player2 = players->player2;
	
	char buffer[1024], *result = NULL;
	do {

	printf("Esperando Mensagem do cliente...\n");
	recv(player1->sock, buffer, sizeof(buffer), 0);

	printf("Mensagem recebida do cliente = %s\n", buffer);

	if (result == NULL ){
		printf("enviando mensagem para os demais clientes, exlcuindo o remetente....\n");

		pthread_mutex_lock(&mutex);
		send(player2->sock,buffer,sizeof(buffer),0);
		pthread_mutex_unlock(&mutex);

	}

	}while (result == NULL);

  pthread_mutex_lock(&mutex);
  socketsThreadsIds[player1->thread] = -1; 
  pthread_mutex_unlock(&mutex);

  printf("fechando conexao...\n");
  shutdown(player1->sock, 2);
  pthread_exit(NULL);

}


int main()
{
    srand(time(NULL));
    int welcomeSocket, chatSocket, newSocket, newChatSocket;
    struct sockaddr_in serverAddr, chatAddr;
    socklen_t addr_size;
    pthread_t threads[NUM_THREADS][THREADS_TUPLE];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    thdata_client data[NUM_THREADS]; // Structs de dados para os jogadores

    // Cria socket principal para o jogo
    welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
    
    // Configuração do endereço para o jogo
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891); // Porta do jogo
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    bind(welcomeSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // Cria socket para o chat
    chatSocket = socket(PF_INET, SOCK_STREAM, 0);
    
    // Configuração do endereço para o chat
    chatAddr.sin_family = AF_INET;
    chatAddr.sin_port = htons(7892); // Porta separada para o chat
    chatAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(chatAddr.sin_zero, '\0', sizeof chatAddr.sin_zero);

    bind(chatSocket, (struct sockaddr *)&chatAddr, sizeof(chatAddr));

    pthread_mutex_init(&mutex, NULL);

    // Escuta até NUM_THREADS conexões no socket do jogo
    if (listen(welcomeSocket, NUM_THREADS) == 0)
        printf("Listening for game connections\n");
    else
        printf("Error in game socket\n");

    // Escuta no socket do chat
    if (listen(chatSocket, NUM_THREADS) == 0)
        printf("Listening for chat connections\n");
    else
        printf("Error in chat socket\n");

    int i = 0;

    for (i = 0; i < NUM_THREADS; i++)
    {
        socketsThreadsIds[i] = -1;
    }

    // Aceita conexões de jogadores e chat
    i = 0;
    while (i < NUM_THREADS)
    {
        printf("esperando conexao do jogador.... \n");
        newSocket = accept(welcomeSocket, (struct sockaddr *)NULL, NULL); // Aceita conexões para o jogo

        pthread_mutex_lock(&mutex);
        socketsThreadsIds[i] = newSocket;
        data[i].game.thread = i;
        data[i].game.sock = newSocket;

        printf("Jogador conectado. Esperando chat...\n");

        // Aceita conexão de chat associada ao jogador
        newChatSocket = accept(chatSocket, (struct sockaddr *)NULL, NULL);
		data[i].chat.thread = i;
        data[i].chat.sock = newChatSocket; // Armazena o socket do chat

        connectedPlayers++;
        printf("Chat para o jogador conectado.\n");

        // Enfileirar e começar o jogo se houver dois jogadores
        fila.push(&data[i]);

        ServerData sendData = convertToChatBuffer("Você é o jogador " + to_string(data[i].game.thread + 1));
        send(data[i].game.sock, &sendData, sizeof(ServerData), 0);

        if (fila.size() % 2 == 0)
        {
            thdata_client *player1 = fila.front();
            fila.pop();
            thdata_client *player2 = fila.front();
            fila.pop();

            PlayerPair *twoPlayersGame = new PlayerPair{&player1->game, &player2->game};
			PlayerPair *twoPlayersChat1 = new PlayerPair{&player1->chat, &player2->chat};
			PlayerPair *twoPlayersChat2 = new PlayerPair{&player2->chat, &player1->chat};

            sendData = convertToChatBuffer("Jogadores conectados. Iniciando jogo...");
            send(player1->game.sock, &sendData, sizeof(ServerData), 0);
            send(player2->game.sock, &sendData, sizeof(ServerData), 0);

            // Cria thread para jogar
            pthread_create(&threads[i][GAME], &attr, &lobby, (void *)twoPlayersGame);

			//cria thread de envio do player 1 ao 2
			pthread_create(&threads[i][CHAT], &attr, &chatRoom, (void *)twoPlayersChat1);
			//cria thread de envio do player 1 ao 2
			pthread_create(&threads[i][CHAT2], &attr, &chatRoom, (void *)twoPlayersChat2);
        }
        else
        {
            ServerData sendData = convertToChatBuffer("Aguardando outro jogador conectar");
            send(data[i].game.sock, &sendData, sizeof(ServerData), 0);
        }

        i++;
        pthread_mutex_unlock(&mutex);
    }

    printf("Abriu todas as threads. Esperando a thread terminar para fechar o servidor.\n");

    for (i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i][GAME], NULL);
		pthread_join(threads[i][CHAT], NULL);
		pthread_join(threads[i][CHAT2], NULL);
    }

    pthread_cond_destroy(&cond_two_players);
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&mutex);

    return 0;
}

/*
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
		data[i].thread = i;
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

	// Clean up and exit
	pthread_cond_destroy(&cond_two_players);
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&mutex);

	return 0;
}

*/