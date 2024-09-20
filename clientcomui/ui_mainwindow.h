/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLineEdit *enterGameGuess;
    QGraphicsView *HangmanImage;
    QTextEdit *chatLogs;
    QTextEdit *textEdit;
    QLabel *labelWrongLetters;
    QLineEdit *ChatEntry;
    QLineEdit *Palavra;
    QLineEdit *ServerMessages;
    QLabel *labelWord;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1097, 644);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        enterGameGuess = new QLineEdit(centralwidget);
        enterGameGuess->setObjectName(QString::fromUtf8("enterGameGuess"));
        enterGameGuess->setGeometry(QRect(50, 550, 551, 31));
        HangmanImage = new QGraphicsView(centralwidget);
        HangmanImage->setObjectName(QString::fromUtf8("HangmanImage"));
        HangmanImage->setGeometry(QRect(20, 190, 341, 271));
        HangmanImage->setStyleSheet(QString::fromUtf8("background: transparent; border: none;"));
        HangmanImage->setInteractive(false);
        chatLogs = new QTextEdit(centralwidget);
        chatLogs->setObjectName(QString::fromUtf8("chatLogs"));
        chatLogs->setGeometry(QRect(720, 0, 371, 521));
        chatLogs->setReadOnly(true);
        textEdit = new QTextEdit(centralwidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(390, 260, 291, 81));
        textEdit->setStyleSheet(QString::fromUtf8("background: transparent; border: none;\n"
"font: 14pt \"Sans Serif\";"));
        textEdit->setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);
        textEdit->setReadOnly(true);
        labelWrongLetters = new QLabel(centralwidget);
        labelWrongLetters->setObjectName(QString::fromUtf8("labelWrongLetters"));
        labelWrongLetters->setGeometry(QRect(420, 230, 211, 31));
        labelWrongLetters->setStyleSheet(QString::fromUtf8("font: 22pt \"Sans Serif\";"));
        ChatEntry = new QLineEdit(centralwidget);
        ChatEntry->setObjectName(QString::fromUtf8("ChatEntry"));
        ChatEntry->setGeometry(QRect(720, 540, 371, 61));
        Palavra = new QLineEdit(centralwidget);
        Palavra->setObjectName(QString::fromUtf8("Palavra"));
        Palavra->setGeometry(QRect(90, 90, 541, 81));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 0));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Active, QPalette::Window, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush);
        Palavra->setPalette(palette);
        Palavra->setAutoFillBackground(false);
        Palavra->setStyleSheet(QString::fromUtf8("font: 28pt \"Sans Serif\";\n"
"background: transparent; border: none;"));
        Palavra->setAlignment(Qt::AlignmentFlag::AlignCenter);
        Palavra->setReadOnly(true);
        ServerMessages = new QLineEdit(centralwidget);
        ServerMessages->setObjectName(QString::fromUtf8("ServerMessages"));
        ServerMessages->setGeometry(QRect(50, 520, 551, 31));
        ServerMessages->setStyleSheet(QString::fromUtf8("background: transparent; border: none;\n"
"font: 14pt \"Sans Serif\";"));
        ServerMessages->setReadOnly(true);
        labelWord = new QLabel(centralwidget);
        labelWord->setObjectName(QString::fromUtf8("labelWord"));
        labelWord->setGeometry(QRect(260, 70, 211, 31));
        labelWord->setStyleSheet(QString::fromUtf8("font: 22pt \"Sans Serif\";"));
        labelWord->setAlignment(Qt::AlignmentFlag::AlignCenter);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1097, 19));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);
        QObject::connect(ChatEntry, &QLineEdit::returnPressed, ChatEntry, qOverload<>(&QLineEdit::clear));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        enterGameGuess->setText(QString());
        chatLogs->setHtml(QCoreApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", nullptr));
        textEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "A - B - C- D - E - F- G - H - I - J - K - L - M - N - O - P - Q - R - S - T - U - V - W - X - Y - Z", nullptr));
        labelWrongLetters->setText(QCoreApplication::translate("MainWindow", "Letras Erradas", nullptr));
        ChatEntry->setText(QString());
        Palavra->setPlaceholderText(QCoreApplication::translate("MainWindow", "PALAVRA", nullptr));
        ServerMessages->setText(QString());
        ServerMessages->setPlaceholderText(QCoreApplication::translate("MainWindow", "Sua vez", nullptr));
        labelWord->setText(QCoreApplication::translate("MainWindow", "Palavra", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
