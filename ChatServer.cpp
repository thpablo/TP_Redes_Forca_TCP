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

using namespace std;

#define NUM_THREADS 30
#define NUM_PLAYERS 2

int socketsThreadsIds[NUM_THREADS];
bool CURRENT_GAME = false;
int playersInConection = 0;

pthread_mutex_t mutex;

typedef struct str_thdata
{
	int thread_no;
	int sock;
} thdata;

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
		for (const auto &word : words)
		{
			std::cout << word << std::endl;
		}
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

// Classe com atributos e logica do jogo
class Hangman
{
private:
	string wordSecret;

protected:
	string wordShown;
	int maxError;
	int contError;

public:
	// Constroi jogo com a palavra na dificuldade dada
	Hangman(string difficulty)
	{
		wordSecret = chooseWord(difficulty); // Escolhe palavra
		cout << "Palavra secreta: " << wordSecret << endl;
		hideWord();						  // Esconde palavra com "_"
		maxError = wordSecret.size() + 2; // Erros maximo = tamanho da palavra + 2
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

	// Desenha partes da forca (com numero maximo de erros)
	void drawPartsOfHangman()
	{
		contError++;
		/*
			logica para desenho
		*/
	}

	// Verifica se ganhou
	bool win()
	{
		return (wordSecret == wordShown) ? true : false;
	}

	// Joga com a palavra ou letra e decide se ganhou ou perdeu
	int play(string word)
	{
		if (check(word))  // Se houve correspondencia a palavra ou letra
			return win(); // Retorna se ganhou
		else
		{
			drawPartsOfHangman(); // Desenha partes do corpo
			// Verifica se houve erros maximos
			if (contError == maxError)
				return -1; // Perde, fim de jogo
		}
		return false; // Nao ganha mas continua jogando
	}

	// Verifica se contem letra ou e a palavra
	bool check(string s)
	{
		bool found = false;

		if (s.size() == 1)
		{ // Verifica letra
			for (int i = 0; i < wordSecret.size(); i++)
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
		return false; // Sem correspondencia
	}
};

/* (NAO EM USO) Valida input com um vetor de inputs validos (mudar posteriormente tipo de vector<string> )
bool isValidInput(string input, vector<string> validInputs)
{
	// Conversao para minusculo
	string lowerInput = input;
	bool res = false;
	std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), [](unsigned char c)
				   { return std::tolower(c); });

	// Retira \n
	if (!lowerInput.empty() && lowerInput.back() == '\n')
	{
		lowerInput.erase(lowerInput.length() - 1);
	}

	// Compara com entradas validas
	for (string v : validInputs)
	{
		if (lowerInput == v) // Se for igual a uma entrada valida
			res = true;
	}

	return res;
}

*/

// Converte Char para String retirando o \n
string convertCharToString(char *c)
{
	string str = c;
    if (!str.empty() && str.back() == '\n') {
        str.pop_back(); // Remove o último caractere
    }
	return str;
}

void *conexao(void *param)
{
	thdata *data;
	data = (thdata *)param; /* type cast to a pointer to thdata */

	int gameStatus = 0; // Status do jogo
	string difficulty;	// armazenar qual dificuldade do jogo
	char buffer[1024];	// buffer da mensagem do cliente
	char msgChooseDifficulty[] = "Escolha a dificuldade para o jogo\n";
	string waitOtherPlayer = playersInConection + " jogadores conectados\n";
	vector<string> validInputs{"facil", "medio", "dificil"}; // dificuldades válidas para o jogo

	/* ---------- Construcao do jogo --------- */
	difficulty = "facil";	  // Mudar para escolha do jogador
	Hangman game(difficulty); // Construcao do jogo

	/* Recebe mensagens enquanto jogo nao acaba */
	do
	{
		// Envia imagem da forca para todos os jogadores
		pthread_mutex_lock(&mutex);

		string wordShown = game.getWordShown();

		// Char para envio em send()
		char wordShownToSend[wordShown.size() + 1];
		// Copia o conteúdo da std::string para o array de char
		strcpy(wordShownToSend, wordShown.c_str());

		// Envia palavra escondida para todos os jogadores
		cout << "Envia palavra escondida para todos os jogadores: " << wordShown << endl;
		for (int i = 0; i < NUM_THREADS; i++)
		{
			if (socketsThreadsIds[i] != -1)
			{
				send(socketsThreadsIds[i], &wordShownToSend, sizeof(wordShownToSend), 0);
			}
		}
		pthread_mutex_unlock(&mutex);

		/* Espera mensagem do cliente */
		cout << "Esperando mensagem do cliente...\n";
		recv(data->sock, buffer, sizeof(buffer), 0);
		cout << "Mensagem recebida do cliente = " << buffer << endl;
		
		// input recebe char convertido para comparacao
		string input = convertCharToString(buffer);

		// Joga com a palavra ou letra e decide se ganhou ou perdeu
		gameStatus = game.play(input);
	} while (gameStatus != -1 && gameStatus != 1); // Enquanto o jogo nao estiver perdido (-1)
												   // ou ganho (1)

	cout << "Status: " << gameStatus << endl;
	cout << "Fim de jogo\n" << endl;

	/* Logica para finalizar threads e acabar jogo */
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

	// Geracao de numeros aleatorios
	srand(time(nullptr));

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
		pthread_create(&threads[i], &attr, &conexao, (void *)&data[i]);

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

// backup para conexao
/*-------------------
		Lógica pra esperar 2 players
	pthread_mutex_lock(&mutex);
	playersInConection++; // Aumenta numero de players conectados

	// Informacao de players conectados
	cout << "Numero de jogadores conectados: " << playersInConection << "\n"<< endl;
	send(socketsThreadsIds[data->thread_no], &playersInConection, sizeof(int), 0);

	pthread_mutex_unlock(&mutex);
	while (playersInConection != 2){};
	---------------------------*/

/* Envio da escolha de dificuldade
printf("Enviando mensagem de escolha de dificuldade para os dois players....\n");
pthread_mutex_lock(&mutex);

// Envia mensagem de dificuldade
send(socketsThreadsIds[data->thread_no], msgChooseDifficulty, sizeof(msgChooseDifficulty), 0);
pthread_mutex_unlock(&mutex);
*/

/* Espera e escolha da dificuldade
recv(data->sock, buffer, sizeof(buffer), 0); // recebe mensagem do cliente
printf("Dificuldade recebida do cliente = %s\n", buffer);

while (!isValidInput(buffer, validInputs)) // Verifica resposta valida
{
	// reenviar mensagem esperando resposta valida
	pthread_mutex_lock(&mutex);
	send(data->sock, msgChooseDifficulty, sizeof(msgChooseDifficulty), 0);
	pthread_mutex_unlock(&mutex);

	// recebe mensagem do cliente
	printf("Aguardando nova tentativa de dificuldade\n");
	recv(data->sock, &buffer, sizeof(buffer), 0);
	printf("Dificuldade recebida do cliente = %s\n", buffer);
}*/
/* Retorna a escolha para o cliente
char replyToClient[] = "Voce escolheu a dificuldade\n";
pthread_mutex_lock(&mutex);
send(data->sock, replyToClient, sizeof(replyToClient), 0);
validDifficultyInput++; // Quantidade de respostas de dificuldade validas
pthread_mutex_unlock(&mutex);

while (validDifficultyInput < 2)
{
};
/**----
 * Lógica para escolher qual dificuldade
 */

/** Escolher palavra */