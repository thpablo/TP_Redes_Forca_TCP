#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>    /* POSIX Threads */ 
#include <stdlib.h>

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
    connectServer();
    connectSignalsAndSlots();
}

MainWindow::~MainWindow()
{
    pthread_join(dataSend.thread, NULL);
    shutdown(dataRecv.sock, 2);
    shutdown(dataSend.sock, 2);
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
        //QObject::connect(ui->enterGameGuess, &QLineEdit::returnPressed, ui->enterGameGuess, qOverload<>(&QLineEdit::clear));
        //QObject::connect(ui->ChatEntry, &QLineEdit::returnPressed, ui->ChatEntry, qOverload<>(&QLineEdit::clear));
}

void* MainWindow::ReceiveMessage(void *param){

  MainWindow *mainWindow;
  mainWindow = (MainWindow *) param;
 
  char buffer[1024];
  //ServerData receivedData;
  int s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  if (s != 0) {
    printf("Erro.");
    exit(0);
  }

  while (mainWindow->sData.flag != WINNER && mainWindow->sData.flag != LOSER){
    recv(mainWindow->dataRecv.sock, &mainWindow->sData, sizeof(ServerData), 0);

    mainWindow->playSound(mainWindow->sData.flag);//toca o som dependendo

    if(mainWindow->sData.flag == WINNER || mainWindow->sData.flag == LOSER){
      break;
    }

    if(mainWindow->sData.isAMessageFromServer == 1){
        QString chatString = QString(mainWindow->sData.chatBuffer);
        mainWindow->ui->ServerMessages->setText(chatString);
      //printf("%s\n",data->sData->chatBuffer);//metadados
    }
    else{
        QString chatString = QString(mainWindow->sData.shownWord);
        mainWindow->ui->Palavra->setText(chatString);
      //printf("Palavra: %s\n",data->sData->shownWord);//chat
    } 
  }
  //função de vitória/derrota
  printf("Fim de Jogo\n");

  return nullptr;

}

void MainWindow::sendGameMessage(){
    memset(cData.buffer, '\0', sizeof(cData.buffer)); // resetar o buffer
    cData.type = GUESS;
    QString qText = ui->enterGameGuess->text();
    ui->enterGameGuess->clear();
    QByteArray byteArray = qText.toUtf8();
    strcpy(cData.buffer, byteArray.constData());

    send(dataSend.sock,&cData,sizeof(ClientData),0);

}


void MainWindow::sendChatMessage(){
    memset(cData.buffer, '\0', sizeof(cData.buffer)); // resetar o buffer
    cData.type = CHAT;
    QString qText = ui->ChatEntry->text();
    ui->ChatEntry->clear();
    QByteArray byteArray = qText.toUtf8();
    strcpy(cData.buffer, byteArray.constData());
    send(dataSend.sock,&cData,sizeof(ClientData),0);
}

void MainWindow::connectServer(){
    int clientSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    pthread_t threadSendId, threadRecvId;
    //thdata dataRecv, dataSend;
    pthread_attr_t attr;


    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  


    addr_size = sizeof serverAddr;
    connect_socket(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));


    printf("criando thread recv...\n");
    dataRecv.sock = clientSocket;
    dataRecv.thread = threadRecvId;


    pthread_create (&threadRecvId, NULL,  MainWindow::ReceiveMessage, (void *) this);

    //printf("criando thread send...\n");
    dataSend.sock = clientSocket;
    //dataSend.thread = threadSendId;
    //pthread_create (&threadSendId, &attr,  &threadSend, (void *) &dataSend);
 
}


// std::string MainWindow::getGameText(){
//     return gameText.toStdString();
// }
// std::string MainWindow::getChatText(){
//     return chatText.toStdString();
// }

void MainWindow::playSound(int type){
  switch (type)
  {
  case RIGHT:
    QSound::play("sfx/right.wav");
    break;
  case WRONG:
    QSound::play("sfx/wrong.wav");
    break;
  case WINNER:
    QSound::play("sfx/victory.wav");
    break;
  case LOSER:
    QSound::play("sfx/loser.wav");
    break;
  default:
    break;
  }
}

void MainWindow::refresh(){
  if (sData.yourTurn == 0) {
      ui->enterGameGuess->setReadOnly(true);  // Impede a escrita
      ui->ServerMessages->setText("Vez do adversário");
   }

  else{
      ui->enterGameGuess->(false); // Libera a escrita
      ui->ServerMessages->setText("Sua vez");
  }
  //QString chatString = QString(mainWindow->sData.shownWord);
  ui->Palavra->setText(QString(sData.shownWord));
  ui->

}