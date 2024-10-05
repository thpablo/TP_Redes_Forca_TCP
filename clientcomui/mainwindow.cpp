#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>    /* POSIX Threads */ 
#include <stdlib.h>
#include <QSoundEffect>

#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <arpa/inet.h>    // htons(), inet_addr()
#include <sys/types.h>    // AF_INET, SOCK_STREAM

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"connectWorkaround.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    int kill = 0;
    gameData.flag = NOTHING;
    connectServer();
    connectSignalsAndSlots();
}

MainWindow::~MainWindow()
{
    int kill = 1;
    delete ui;
}

void MainWindow::connectSignalsAndSlots(){
    //     QObject::connect(ui->enterGameGuess, &QLineEdit::returnPressed, [=](){
    //     QString gameText = ui->enterGameGuess->text();  // Obtém o texto do QLineEdit
    //     //QMessageBox::information(nullptr, "Texto", "O texto digitado é: " + gameText);
    // });
    //     QObject::connect(ui->ChatEntry, &QLineEdit::returnPressed, [=]() {
    //     QString chatText = ui->ChatEntry->text();  // Obtém o texto do QLineEdit
    // });
        QObject::connect(ui->enterGameGuess, &QLineEdit::returnPressed, this, &MainWindow::sendGameMessage);
        QObject::connect(ui->ChatEntry, &QLineEdit::returnPressed, this, &MainWindow::sendChatMessage);
        QObject::connect(this, &MainWindow::newChatMessageReceived, this, &MainWindow::appendChatMessage);
        QObject::connect(this, &MainWindow::newGameData, this, &MainWindow::refreshGame);
        //QObject::connect(ui->enterGameGuess, &QLineEdit::returnPressed, ui->enterGameGuess, qOverload<>(&QLineEdit::clear));
        //QObject::connect(ui->ChatEntry, &QLineEdit::returnPressed, ui->ChatEntry, qOverload<>(&QLineEdit::clear));
}

void* ReceiveGameData(void *param){

  MainWindow *mainWindow;
  mainWindow = (MainWindow *) param;
 
  //ServerData receivedData;
  int s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  if (s != 0) {
    printf("Erro.");
    exit(0);
  }

  while (mainWindow->gameData.flag != WINNER && mainWindow->gameData.flag != LOSER && mainWindow->kill != 1){
    if(recv(mainWindow->thDataGame.sock, &mainWindow->gameData, sizeof(ServerData), 0) <= 0){
        //conexão perdida
        emit mainWindow->newChatMessageReceived(QString("ERRO: CONEXÃO PERDIDA, DESCONECTANDO..."));
        // sleep(2);
        // shutdown(mainWindow->dataRecv.sock, 2);
        // pthread_exit(nullptr);  // Encerrar a thread de recebimento
        // return nullptr;
        pthread_cancel(mainWindow->thDataGame.thread);
        sleep(5);
        shutdown(mainWindow->thDataGame.sock, 2);
        pthread_exit(NULL);
    }

    emit mainWindow->newGameData(mainWindow->gameData);

    if(mainWindow->gameData.flag == WINNER || mainWindow->gameData.flag == LOSER){
      break;
    }


  }
  //função de vitória/derrota
    printf("Fim de Jogo\n");
    pthread_cancel(mainWindow->thDataGame.thread);
    sleep(5);
    shutdown(mainWindow->thDataGame.sock, 2);
    pthread_exit(NULL);

}

void* ReceiveChatData(void *param){

  MainWindow *mainWindow;
  mainWindow = (MainWindow *) param;
 
  char buffer[1024];
  int s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  if (s != 0) {
    printf("Erro.");
    exit(0);
  }

  while (1){
    recv(mainWindow->thDataChat.sock, buffer, sizeof(buffer), 0);
    emit mainWindow->newChatMessageReceived(QString(buffer));
  }
    printf("Fim de Jogo\n");
    pthread_cancel(mainWindow->thDataGame.thread);
    sleep(5);
    shutdown(mainWindow->thDataGame.sock, 2);
    pthread_exit(NULL);
}

void MainWindow::sendGameMessage(){
    memset(cData.buffer, '\0', sizeof(cData.buffer)); // resetar o buffer
    cData.type = GUESS;
    QString qText = ui->enterGameGuess->text();
    ui->enterGameGuess->clear();
    QByteArray byteArray = qText.toUtf8();
    strcpy(cData.buffer, byteArray.constData());

    send(thDataGame.sock,&cData,sizeof(ClientData),0);

}


// void MainWindow::sendChatMessage(){
//     memset(cData.buffer, '\0', sizeof(cData.buffer)); // resetar o buffer
//     cData.type = CHAT;
//     QString qText = ui->ChatEntry->text();
//     ui->ChatEntry->clear();
//     QByteArray byteArray = qText.toUtf8();
//     strcpy(cData.buffer, byteArray.constData());
//     send(dataSend.sock,&cData,sizeof(ClientData),0);
// }

void MainWindow::sendChatMessage(){
    memset(cData.buffer, '\0', sizeof(cData.buffer)); // resetar o buffer
    cData.type = CHAT;
    chatLog.append("Você: " + ui->ChatEntry->text());
    QString qText = name + ": " + ui->ChatEntry->text();
    ui->ChatEntry->clear();
    QByteArray byteArray = qText.toUtf8();
    strcpy(cData.buffer, byteArray.constData());
    send(thDataChat.sock,&cData,sizeof(ClientData),0);
}

void MainWindow::connectServer(){
    int gameSocket, chatSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    pthread_t threadRecvGameId, threadRecvChatId;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Criando o socket para o jogo
    gameSocket = socket(PF_INET, SOCK_STREAM, 0);
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);  // Porta para o jogo
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));  

    addr_size = sizeof serverAddr;
    connect_socket(gameSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    printf("Conectando ao servidor de jogo...\n");
    thDataGame.sock = gameSocket;  // Armazena o socket de jogo
    thDataGame.thread = threadRecvGameId;

    // Criando thread para receber dados do jogo
    pthread_create(&threadRecvGameId, NULL, ReceiveGameData, (void *)this);

    // Criando o segundo socket para o chat
    chatSocket = socket(PF_INET, SOCK_STREAM, 0);
    
    serverAddr.sin_port = htons(7892);  // Porta para o chat, diferente da porta do jogo
    connect_socket(chatSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    printf("Conectando ao servidor de chat...\n");
    thDataChat.sock = chatSocket;  // Armazena o socket de chat
    thDataChat.thread = threadRecvChatId;

    // Criando thread para receber mensagens de chat
    pthread_create(&threadRecvChatId, NULL, ReceiveChatData, (void *)this);

}

    // int clientSocket;
    // struct sockaddr_in serverAddr;
    // socklen_t addr_size;
    // pthread_t threadSendId, threadRecvId;
    // //thdata dataRecv, dataSend;


    // pthread_attr_init(&attr);
    // pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    
    // serverAddr.sin_family = AF_INET;
    // serverAddr.sin_port = htons(7891);
    // serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  


    // addr_size = sizeof serverAddr;
    // connect_socket(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));


    // printf("criando thread recv...\n");
    // dataRecv.sock = clientSocket;
    // dataRecv.thread = threadRecvId;


    // pthread_create (&threadRecvId, NULL,  ReceiveGameData, (void *) this);

   
    // dataSend.sock = clientSocket;
    
    
 



// std::string MainWindow::getGameText(){
//     return gameText.toStdString();
// }
// std::string MainWindow::getChatText(){
//     return chatText.toStdString();
// }


void MainWindow::playSound(int type) {
    QSoundEffect* soundEffect = new QSoundEffect(this); // Criar um novo QSoundEffect

    switch (type) {
    case RIGHT:
        soundEffect->setSource(QUrl::fromLocalFile("sfx/right.wav"));
        break;
    case WRONG:
        soundEffect->setSource(QUrl::fromLocalFile("sfx/wrong.wav"));
        break;
    case WINNER:
        soundEffect->setSource(QUrl::fromLocalFile("sfx/victory.wav"));
        break;
    case LOSER:
        soundEffect->setSource(QUrl::fromLocalFile("sfx/loser.wav"));
        break;
    default:
        delete soundEffect; // Limpar se não houver som
        return;
    }

    soundEffect->setVolume(1.0f); // Definir o volume (0.0 a 1.0)
    soundEffect->play(); // Reproduzir o som
}


void MainWindow::refresh(){
  if (gameData.yourTurn == 0) {
      ui->enterGameGuess->setReadOnly(true);  // Impede a escrita
      ui->ServerMessages->setText("Vez do adversário");
   }

  else{
      ui->enterGameGuess->setReadOnly(false); // Libera a escrita
      ui->ServerMessages->setText("Sua vez");
  }
  //QString chatString = QString(mainWindow->gameData.shownWord);
  ui->Palavra->setText(QString(gameData.shownWord));
  ui->wrongLetters->setText(QString(gameData.wrongLetters));

}

void MainWindow::refreshGame(const ServerData gameData){
}

void MainWindow::appendChatMessage(const QString &message){
    ui->chatLogs->append(message);
}