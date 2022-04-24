#include "mainwindow.h"
#include "QAction"
#include "ui_mainwindow.h"

#include "QMessageBox"
#include "QApplication"
#include "QDebug"

#include <QMetaType>
#include <QAction>

#include <vector>

#include <QPair>
#include <QSet>

#include <QStyleFactory>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->modulesTabs->addTab(new Tabs, QString("FAQ"));
    ui->modulesTabs->setTabsClosable(false);

    QSqlDatabase testdb=QSqlDatabase::addDatabase("QSQLITE");
    testdb.setDatabaseName("E:/Projects/sqlite/testdb.db");

    this->setStyleSheet("QMenuBar {background-color: yellow ; border: 2px solid red; margin: 0; padding: 5px; border-radius: 20px; background: white;}");

    setWindowTitle("Solve1");
    setStyleSheet("background-color: light gray");

    login = "unauthorized";

    on_buttonToAuth_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotFabriqObjects(QString moduleName)
{
    if(moduleName == "faq")  {ui->modulesTabs->addTab(new Tabs, moduleName);}
    if(moduleName == "sqrt") {ui->modulesTabs->addTab(new moduleSqrt, moduleName);}
    if(moduleName == "sqr")  {ui->modulesTabs->addTab(new moduleSqr, moduleName);}
    if(moduleName == "impl") {ui->modulesTabs->addTab(new moduleImplement, moduleName);}
    if(moduleName == "adm")  {ui->modulesTabs->addTab(new moduleAdm, moduleName);}
    ui->modulesTabs->setCurrentIndex(ui->modulesTabs->count() - 1);
    ui->modulesTabs->setTabsClosable(1);
}

void MainWindow::on_buttonToAuth_clicked() //auth has been started
{
    windowAuth = new Auth();
    windowAuth->show();
    connect(windowAuth, &Auth::signalLoginStatus, this, &MainWindow::slotLoginStatus);
}

void MainWindow::slotLoginStatus(int codeOfResult, QString loginAuth, QString rolesList)
{
    ui->menubar->clear();

    userBar->clear();

    QMenu *userMenu = new QMenu(loginAuth, userBar);
    QAction *newAction = new QAction();

    newAction->setText("faq");
    connect(newAction, &QAction::triggered, this, [=](){slotFabriqObjects("faq");});

    QAction *logInAction = new QAction("Войти");
    connect(logInAction, &QAction::triggered, this, [=](){
        windowAuth = new Auth();
        windowAuth->show();
        connect(windowAuth, &Auth::signalLoginStatus, this, &MainWindow::slotLoginStatus);
    });

    userBar->addMenu(userMenu);

    userBar->setVisible(1);
    ui->menubar->setCornerWidget(userBar);

    login = loginAuth;

    ui->toolBar->clear();
    ui->toolBar->addAction(newAction);
    ui->modulesTabs->clear();
    ui->modulesTabs->addTab(new Tabs, QString("FAQ"));
    ui->modulesTabs->setTabsClosable(false);

    /*
     * Добавить валидатор паролей
     * Добавить изкоробки пункт выхода в меню File
     * */

    if(codeOfResult == 1)
    {
        show();

        logInAction->setText("Сменить пользователя");
        userMenu->addAction(logInAction);
        windowAuth->hide();
        authorized = 1;

        vector<QString> listOfModules = listOfModulesToBar(rolesList);//listOfModulesToBar(roleData[loginRole[login]]);
        QSet<QString> checkMenu; //flags for every meny (exist or not)
        vector<QPair<QString, QPair<QString, QString>>> dynamicMenu; //nameOfObj, type(QAction or QMenu), parent menu ("Main" if main tab)
        int processed = 0; //count of processed points in dynamicMenu

        checkMenu.clear();
        dynamicMenu.clear();

        sort(listOfModules.begin(), listOfModules.end());
        for(unsigned long long i = 0; i < listOfModules.size(); i++)
        {
            QAction *newAction = new QAction();
            newAction->setText(listOfModules[i]);
            connect(newAction, &QAction::triggered, this, [=](){
                slotFabriqObjects(listOfModules[i]);
            });
            ui->toolBar->addAction(newAction);

            QSqlQuery qryActionPath;
            QString actionPath;
            if(qryActionPath.exec("select * from actionPath where action='" + listOfModules[i] + "'"))
            {
                qryActionPath.next();
                actionPath = qryActionPath.value(qryActionPath.record().indexOf("path")).toString();
            }

            vector<QString> partsOfPath = listOfModulesToBar(actionPath);

            for(unsigned long long j = 0; j < partsOfPath.size(); j++)
            {
                int isNew = checkMenu.size();
                if(j != partsOfPath.size() - 1)
                {
                    checkMenu.insert(partsOfPath[j]);
                    if(isNew < checkMenu.size())
                    {
                        if(j == 0)
                            dynamicMenu.push_back(make_pair(partsOfPath[j], make_pair("QMenu", "Main")));
                        else
                            dynamicMenu.push_back(make_pair(partsOfPath[j], make_pair("QMenu", partsOfPath[j - 1])));
                    }
                }
                else if(j == partsOfPath.size() - 1)
                    dynamicMenu.push_back(make_pair(partsOfPath[j], make_pair("QAction", partsOfPath[j - 1])));
            }
        }

        vector<QPair<QString, QMenu*>> dynamicMenuForShow;
        QSet<QString> addedMenus;
        QSet<QString> MainMenus;

        for(unsigned long long i = 0; i < dynamicMenu.size(); i++)
        {
            if(dynamicMenu[i].second.second == "Main")
            {
                QMenu *newMainMenu = new QMenu;
                dynamicMenuForShow.push_back(make_pair(dynamicMenu[i].first, newMainMenu));
                addedMenus.insert(dynamicMenu[i].first);
                MainMenus.insert(dynamicMenu[i].first);
                processed++;
            }
        }
        while(processed != int(dynamicMenu.size()))
        {
            for(unsigned long long i = 0; i < dynamicMenu.size(); i++)
            {
                if(dynamicMenu[i].second.first != "QAction" && addedMenus.find(dynamicMenu[i].first) == addedMenus.end() && addedMenus.find(dynamicMenu[i].second.second) != addedMenus.end())
                {
                    int pointer = -1;
                    for(unsigned long long j = 0; j < dynamicMenuForShow.size(); j++)
                    {
                        if(dynamicMenuForShow[j].first == dynamicMenu[i].second.second)
                        {
                            pointer = j;
                            break;
                        }
                    }

                    QMenu *newMenu = new QMenu;
                    newMenu->setTitle(dynamicMenu[i].first);
                    dynamicMenuForShow.push_back(make_pair(dynamicMenu[i].first, newMenu));
                    addedMenus.insert(dynamicMenu[i].first);
                    processed++;

                    dynamicMenuForShow[pointer].second->addMenu(newMenu);
                }
                else if(dynamicMenu[i].second.first == "QAction")
                    processed++;
            }
        }

        for(unsigned long long i = 0; i < dynamicMenu.size(); i++)
        {
            if(dynamicMenu[i].second.first == "QAction")
            {
                for(unsigned long long j = 0; j < dynamicMenuForShow.size(); j++)
                {
                    if(dynamicMenuForShow[j].first == dynamicMenu[i].second.second)
                    {
                        QAction *newAction = new QAction();
                        newAction->setText(dynamicMenu[i].first);
                        connect(newAction, &QAction::triggered, this, [=](){
                            slotFabriqObjects(dynamicMenu[i].first);
                        });

                        dynamicMenuForShow[j].second->addAction(newAction);
                        break;
                    }
                }

            }
        }

        for(unsigned long long i = 0; i < dynamicMenuForShow.size(); i++)
        {
            if(MainMenus.find(dynamicMenuForShow[i].first) != MainMenus.end())
            {
                dynamicMenuForShow[i].second->setTitle(dynamicMenuForShow[i].first);
                ui->menubar->addMenu(dynamicMenuForShow[i].second);
            }
        }

    }
    else
    {
        userMenu->addAction(logInAction);
        show();
        windowAuth->hide();
        authorized = 0;
        QMessageBox::warning(this, "Внимание", "Вы не вошли в свою запись. Функционал ограничен");
    }

    QAction *exitAction = new QAction("Выйти");
    connect(exitAction, &QAction::triggered, this, [=](){QApplication::quit();});

    userMenu->addAction(exitAction);
}

void MainWindow::on_modulesTabs_tabCloseRequested(int index)
{
    ui->modulesTabs->removeTab(index);
    if(!ui->modulesTabs->count())
    {
        slotFabriqObjects("faq");
        ui->modulesTabs->setTabsClosable(false);
    }
}
