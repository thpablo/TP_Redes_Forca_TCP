/********************************************************************************
** Form generated from reading UI file 'loginDialogBox.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOGBOX_H
#define UI_LOGINDIALOGBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QPushButton *continueButton;
    QPushButton *sairButton;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QTextEdit *nomeTextEdit;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_2;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName(QString::fromUtf8("LoginDialog"));
        LoginDialog->resize(400, 300);
        LoginDialog->setContextMenuPolicy(Qt::ContextMenuPolicy::DefaultContextMenu);
        continueButton = new QPushButton(LoginDialog);
        continueButton->setObjectName(QString::fromUtf8("continueButton"));
        continueButton->setGeometry(QRect(270, 240, 75, 23));
        sairButton = new QPushButton(LoginDialog);
        sairButton->setObjectName(QString::fromUtf8("sairButton"));
        sairButton->setGeometry(QRect(190, 240, 75, 23));
        verticalLayoutWidget = new QWidget(LoginDialog);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(30, 103, 341, 101));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_2->addWidget(label);

        nomeTextEdit = new QTextEdit(verticalLayoutWidget);
        nomeTextEdit->setObjectName(QString::fromUtf8("nomeTextEdit"));
        nomeTextEdit->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        nomeTextEdit->setStyleSheet(QString::fromUtf8("font: 18pt \"Sans Serif\";"));
        nomeTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        nomeTextEdit->setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);

        horizontalLayout_2->addWidget(nomeTextEdit);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));

        verticalLayout->addLayout(horizontalLayout_3);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout->addItem(verticalSpacer_2);

        label_2 = new QLabel(LoginDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(90, 10, 281, 51));
        label_2->setStyleSheet(QString::fromUtf8("font: 24pt \"Sans Serif\";"));
#if QT_CONFIG(shortcut)
        label->setBuddy(nomeTextEdit);
#endif // QT_CONFIG(shortcut)

        retranslateUi(LoginDialog);
        QObject::connect(sairButton, &QPushButton::clicked, LoginDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "Dialog", nullptr));
        continueButton->setText(QCoreApplication::translate("LoginDialog", "&continuar", nullptr));
        sairButton->setText(QCoreApplication::translate("LoginDialog", "&sair", nullptr));
        label->setText(QCoreApplication::translate("LoginDialog", "Nome:", nullptr));
        nomeTextEdit->setPlaceholderText(QString());
        label_2->setText(QCoreApplication::translate("LoginDialog", "Jogo da Forca", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOGBOX_H
