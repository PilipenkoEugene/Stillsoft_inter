#ifndef AUTH_H
#define AUTH_H

#include <QDialog>
#include <QtSql>
#include <QtDebug>
#include <QFileInfo>

namespace Ui {
class Auth;
}

class Auth : public QDialog
{
    Q_OBJECT

public:
    int statusLogin = 0;

public:
    explicit Auth(QWidget *parent = nullptr);
    ~Auth();

public slots:
    void on_buttonAuth_clicked();

private:
    Ui::Auth *ui;
    QSqlDatabase loginData;

signals:
    void signalLoginStatus(int statusLogin, QString login, QString rolesList);

private slots:
    void on_buttonAuthLater_clicked();
};

#endif // AUTH_H
