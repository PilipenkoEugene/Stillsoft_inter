#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <auth.h>
#include <tabs.h>
#include <modulesqrt.h>
#include <modulesqr.h>
#include <moduleimplement.h>
#include <moduleadm.h>
#include <QtAlgorithms>
#include <QTableWidgetItem>
#include <QtDebug>
#include <QFileInfo>
#include <QMenuBar>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    bool authorized = 0;  //status of auth

    vector<QPair<QString, QString>> loginData;  //log and pass
    QMap<QString, int> loginRole;  //log and role
    QMap<int, QString> roleData;  //role and functions

    QMenuBar *userBar = new QMenuBar();

    QString login = "";
    int role = 0;
    QString roleCurrent = "";

public:
    vector<QPair<QString, QString>>  getLoginData()  //return vector of log+pass while SQL-table isn't exist
    {
        return loginData;
    }

    vector<QString> listOfModulesToBar(QString modules)
    {
        vector<QString> listOfModules;
        listOfModules.clear();

        QString findModule = "";
        for(int i = 0; i < modules.size(); i++)
            if(modules[i] != '|')
                findModule += modules[i];
            else
            {
                listOfModules.push_back(findModule);
                findModule = "";
            }
        if(modules.length())
            listOfModules.push_back(findModule);
        return listOfModules;
    }

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_buttonToAuth_clicked();
    void on_modulesTabs_tabCloseRequested(int index);

private:
    Ui::MainWindow *ui;
    Auth *windowAuth;

signals:
    void runAdm(int id, QString log, QString pass, QString roleD);

public slots:
    void slotLoginStatus(int codeOfResult, QString loginAuth, QString rolesList);
    void slotFabriqObjects(QString moduleName);

};
#endif // MAINWINDOW_H
