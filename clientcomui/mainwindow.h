#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include<string>
#include"../data.h"

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
QString name;

thdata thDataGame, thDataChat;
ClientData cData;
//ServerData gameData;

pthread_attr_t attr;

int kill;

void connectSignalsAndSlots();
void connectServer();
void playSound(int type);
void changeImage(int qtdErrors);
public:
    friend void* ReceiveGameData(void *param);
    friend void* ReceiveChatData(void *param);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setName(const QString &text){
        name = text;
    }

// private slots:
//     void on_enterGameGuess_returnPressed();

signals:
    void newGameData(const ServerData gameData);
    void newChatMessageReceived(const QString &message);

public slots:
    void refreshGame(const ServerData gameData);
    void appendChatMessage(const QString &message);
    void sendGameMessage();
    void sendChatMessage();


private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H

