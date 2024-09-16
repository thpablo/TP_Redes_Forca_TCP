#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<string>
//#include"data.h"

//mudat depois
typedef enum{CHAT, GAME}ClientDataType;
typedef enum{RIGHT, WRONG, WINNER, LOSER}FlagType;

//dados do cliente ao server
typedef struct clientdata{
    ClientDataType type;//0 = chat, 1 =guess
    char buffer[1024];
}ClientData;

//dados enviados do server aos clientes, adicione o que for necessário
typedef struct serverdata{
    char chatBuffer[1024];
    char shownWord[100];
    int isAMessageFromServer; // 0 = Printa palavra escondida, 1 = Printa buffer do chat
    FlagType flag;
    // char wrongLetters[26];

}ServerData;

typedef struct str_thdata{
  int sock;
  pthread_t thread;
} thdata;



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

QString gameText, chatText; //input do usuário
QString hiddenWord, chatLog, wrongLetters;//output para o usuário

thdata dataRecv, dataSend;
ClientData cData;
ServerData sData;

void connectSignalsAndSlots();
void connectServer();
static void* ReceiveMessage(void *param);
public:
    
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    std::string getGameText();
    std::string getChatText();

// private slots:
//     void on_enterGameGuess_returnPressed();

public slots:
    void sendGameMessage();
    void sendChatMessage();


private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H

