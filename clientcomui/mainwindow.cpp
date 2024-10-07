#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>    /* POSIX Threads */ 
#include <stdlib.h>
#include <QSoundEffect>
#include<QMessageBox>

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
    running = true; // Inicia a variável de controle
    connectServer();
    connectSignalsAndSlots();
    ui->HangmanImage->setScene(new QGraphicsScene(this));
    changeImage(0);
}

MainWindow::~MainWindow()
{
    running = false; // Sinaliza para as threads que devem encerrar

    // Aguarda a thread de jogo finalizar
    pthread_join(thDataGame.thread, NULL);

    // Aguarda a thread de chat finalizar
    pthread_join(thDataChat.thread, NULL);

    delete ui;
}

void MainWindow::connectSignalsAndSlots(){
        QObject::connect(ui->enterGameGuess, &QLineEdit::returnPressed, this, &MainWindow::sendGameMessage);
        QObject::connect(ui->ChatEntry, &QLineEdit::returnPressed, this, &MainWindow::sendChatMessage);
        QObject::connect(this, &MainWindow::newChatMessageReceived, this, &MainWindow::appendChatMessage);
        QObject::connect(this, &MainWindow::newGameData, this, &MainWindow::refreshGame);
    
}

void* ReceiveGameData(void *param) {
    MainWindow *mainWindow = (MainWindow *) param;
    ServerData gameData;
    gameData.flag = NOTHING;

    while (mainWindow->running) {
        if (recv(mainWindow->thDataGame.sock, &gameData, sizeof(ServerData), 0) <= 0) {
            emit mainWindow->newChatMessageReceived(QString("ERRO: CONEXÃO PERDIDA, DESCONECTANDO..."));
            break;
        }

        if (gameData.isAMessageFromServer)
            emit mainWindow->newChatMessageReceived(QString(gameData.chatBuffer));
        else {
            emit mainWindow->newGameData(gameData);
        }

        if (gameData.flag == WINNER || gameData.flag == LOSER) 
            break;
        
    }

    printf("Fim de Jogo\n");
    shutdown(mainWindow->thDataGame.sock, 2);
    pthread_exit(NULL);
}

void* ReceiveChatData(void *param) {
    MainWindow *mainWindow = (MainWindow *) param;
    char buffer[1024];

    while (mainWindow->running) {
        if (recv(mainWindow->thDataChat.sock, buffer, sizeof(buffer), 0) <= 0)
            break;

        emit mainWindow->newChatMessageReceived(QString(buffer));
    }

    printf("Fim de Jogo\n");
    shutdown(mainWindow->thDataChat.sock, 2);
    pthread_exit(NULL);
}

void MainWindow::sendGameMessage(){
    memset(cData.buffer, '\0', sizeof(cData.buffer)); // resetar o buffer
    cData.type = GAME;
    QString qText = ui->enterGameGuess->text();
    ui->enterGameGuess->clear();
    QByteArray byteArray = qText.toUtf8();
    strcpy(cData.buffer, byteArray.constData());


    
    send(thDataGame.sock,&cData,sizeof(ClientData),0);

}

void MainWindow::sendChatMessage(){
    memset(cData.buffer, '\0', sizeof(cData.buffer)); // resetar o buffer
    emit newChatMessageReceived("Você: " + ui->ChatEntry->text());
    QString qText = name + ": " + ui->ChatEntry->text();
    ui->ChatEntry->clear();
    char buffer[1024];
    QByteArray byteArray = qText.toUtf8();
    strcpy(buffer, byteArray.constData());
    send(thDataChat.sock, buffer ,sizeof(buffer),0);
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
    serverAddr.sin_addr.s_addr = inet_addr("192.168.2.4");
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
    case NOTHING:
        delete soundEffect; // Limpar se não houver som
        return;
        break;
    default:
        delete soundEffect; // Limpar se não houver som
        return;
    }

    soundEffect->setVolume(1.0f); // Definir o volume (0.0 a 1.0)
    soundEffect->play(); // Reproduzir o som
}


void MainWindow::refreshGame(const ServerData gameData){
    playSound(gameData.flag);
    changeImage(gameData.wrongGuesses);
    if (gameData.yourTurn == 0) {
        ui->enterGameGuess->setReadOnly(true);  // Impede a escrita
        ui->ServerMessages->setText("Vez do adversário");
    }

    else{
        ui->enterGameGuess->setReadOnly(false); // Libera a escrita
        ui->ServerMessages->setText("Sua vez");
    }
    
    ui->Palavra->setText(QString(gameData.shownWord).toUpper());
    ui->wrongLetters->setText(QString(gameData.wrongLetters).toUpper());

    if(gameData.flag == WINNER){
        ui->enterGameGuess->setReadOnly(true);  // Impede a escrita
        ui->ServerMessages->setText("Você Venceu!!!");
    }
    else if(gameData.flag == LOSER){
        ui->enterGameGuess->setReadOnly(true);  // Impede a escrita
        ui->ServerMessages->setText("Você Perdeu!!!");
    }
}

void MainWindow::appendChatMessage(const QString &message){
        ui->chatLogs->append(message);

}

void MainWindow::changeImage(int qtdErrors) {
    ui->HangmanImage->scene()->clear();  // Limpa a cena atual

    QPixmap pixmap;  // Declara a variável pixmap antes do switch

    switch (qtdErrors) {
        case 0:
            pixmap = QPixmap("images/e0.png");  // Atribui a imagem correspondente
            break;
        case 1:
            pixmap = QPixmap("images/e1.png");
            break;
        case 2:
            pixmap = QPixmap("images/e2.png");
            break;
        case 3:
            pixmap = QPixmap("images/e3.png");
            break;
        case 4:
            pixmap = QPixmap("images/e4.png");
            break;
        case 5:
            pixmap = QPixmap("images/e5.png");
            break;
        case 6:
            pixmap = QPixmap("images/e6.png");
            break;
        default:
            pixmap = QPixmap();  // Caso de erro: nenhuma imagem
            break;
    }

    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);  // Cria o item gráfico com a imagem
    ui->HangmanImage->scene()->addItem(item);  // Adiciona o item à cena
    ui->HangmanImage->fitInView(item, Qt::KeepAspectRatio);  // Ajusta a visualização para a imagem
}
