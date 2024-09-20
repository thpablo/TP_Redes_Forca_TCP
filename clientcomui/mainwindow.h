#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

