#include "loginDialogBox.h"
#include "ui_loginDialogBox.h"
#include<QMessageBox>
#include<QCloseEvent>
#include<QKeyEvent>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

//tela para o usuário entrar com próprio nome
void LoginDialog::on_continueButton_clicked(){
    QString nome = ui->nomeTextEdit->toPlainText();
    QString resultStr;
    
    if(!nome.isEmpty()){
        emit textSent(nome);
        resultStr = "BEM VINDO";
        this->accept();
    }
    else{
        resultStr = "NOME INVÁLIDO";
        QMessageBox::information(this, "Login", resultStr, QMessageBox::Close);

    }

}


