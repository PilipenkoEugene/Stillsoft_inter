#include "auth.h"
#include "ui_auth.h"

#include "QMessageBox"

using namespace std;

Auth::Auth(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Auth)
{
    ui->setupUi(this);
    setWindowTitle("Авторизация");

    loginData = QSqlDatabase::addDatabase("QSQLITE");
    loginData.setDatabaseName("E:/Projects/sqlite/testdb.db");
    loginData.open();
}

Auth::~Auth()
{
    delete ui;
}

void Auth::on_buttonAuth_clicked()  //auth or skip this part
{
    statusLogin = 0;  //0 - skiped part, 1 - log/pass correct, 2 - incorrect pass, 3 - incorrect login
    QSqlQuery qryLogin;
    QString log, pass;

    if(qryLogin.exec("select * from loginData where login='" + ui->lineLogin->text() + "' and password='" + ui->linePassword->text() + "'"))
    {
        qryLogin.next();

        log = qryLogin.value(qryLogin.record().indexOf("login")).toString();
        pass = qryLogin.value(qryLogin.record().indexOf("password")).toString();

        if(ui->lineLogin->text() == log && ui->linePassword->text() == pass)
        {
            statusLogin = 1;
            qryLogin.exec("select * from loginRole where login='" + ui->lineLogin->text() + "'");
            qryLogin.next();
            int roleId = qryLogin.value(qryLogin.record().indexOf("roleId")).toInt();

            qryLogin.exec("select * from roleData where roleId='" + QString::number(roleId) + "'");
            qryLogin.next();
            QString rolesList = qryLogin.value(qryLogin.record().indexOf("rolesList")).toString();

            //QMessageBox::warning(this, "a", rolesList);
            emit signalLoginStatus(statusLogin, ui->lineLogin->text(), rolesList);
            return;
        }
        else if(ui->lineLogin->text() == log && ui->linePassword->text() != pass)
            statusLogin = 2;
        else if(ui->lineLogin->text() != log && statusLogin != 2)
            statusLogin = 3;

        if(statusLogin == 2)
            QMessageBox::warning(this, "Error", "Wrong password");
        else
            QMessageBox::warning(this, "Error", "Login is incorrect");
   }
}

void Auth::on_buttonAuthLater_clicked() //auth isn't finished
{
    statusLogin = 0;
    emit signalLoginStatus(statusLogin, "Не авторизован", "");
    return;
}

