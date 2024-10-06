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
    connectServer();
    connectSignalsAndSlots();
    ui->HangmanImage->setScene(new QGraphicsScene(this));
    changeImage(0);
}

MainWindow::~MainWindow()
{
    //int kill = 1;
    // Finalizar as threads de jogo e chat
    pthread_cancel(thDataGame.thread);  // Cancela a thread de jogo
    pthread_join(thDataGame.thread, NULL);  // Aguarda a thread de jogo finalizar

    pthread_cancel(thDataChat.thread);  // Cancela a thread de chat
    pthread_join(thDataChat.thread, NULL);  // Aguarda a thread de chat finalizar

    // Fechar os sockets de jogo e chat
    if (thDataGame.sock != -1) {
        shutdown(thDataGame.sock, 2);  // Fecha o socket de jogo
        thDataGame.sock = -1;    // Define como inválido após fechar
    }

    if (thDataChat.sock != -1) {
        shutdown(thDataChat.sock, 2);  // Fecha o socket de chat
        thDataChat.sock = -1;    // Define como inválido após fechar
    }
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
    ServerData gameData;

    gameData.flag = NOTHING;

    while (gameData.flag != WINNER && gameData.flag != LOSER && mainWindow->kill != 1){
        if(recv(mainWindow->thDataGame.sock, &gameData, sizeof(ServerData), 0) <= 0){
            //conexão perdida
            emit mainWindow->newChatMessageReceived(QString("ERRO: CONEXÃO PERDIDA, DESCONECTANDO..."));
            break;
            // sleep(2);
            // shutdown(mainWindow->dataRecv.sock, 2);
            // pthread_exit(nullptr);  // Encerrar a thread de recebimento
            // return nullptr;
            // sleep(5);
            // pthread_cancel(mainWindow->thDataGame.thread);
            // shutdown(mainWindow->thDataGame.sock, 2);
            // pthread_exit(NULL);
        }

        if(gameData.isAMessageFromServer)
            emit mainWindow->newChatMessageReceived(QString(gameData.chatBuffer));
        
        else{
            emit mainWindow->newGameData(gameData);
        }
        //emit mainWindow->newGameData(gameData);

        if(gameData.flag == WINNER || gameData.flag == LOSER){
            emit mainWindow->newGameData(gameData);
            break;
        }

    }
    //função de vitória/derrota
        printf("Fim de Jogo\n");
        sleep(5);
        // pthread_cancel(mainWindow->thDataGame.thread);
        // shutdown(mainWindow->thDataGame.sock, 2);
        // pthread_exit(NULL);
        return nullptr;

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
        //emit mainWindow->newChatMessageReceived(QString(buffer));
}
    printf("Fim de Jogo\n");
    // pthread_cancel(mainWindow->thDataGame.thread);
    // sleep(5);
    // shutdown(mainWindow->thDataGame.sock, 2);
    // pthread_exit(NULL);
    return nullptr;
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
    //QString chatString = QString(mainWindow->gameData.shownWord);
    ui->Palavra->setText(QString(gameData.shownWord).toUpper());
    
    ui->wrongLetters->setText(QString(gameData.wrongLetters).toUpper());
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
