#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "initialization.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //список чатов
    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setDatabaseName("fn1131_2021");
    m_db.setHostName    ("195.19.32.74");
    m_db.setPort        (5432);
    m_db.setUserName    ("student");
    m_db.setPassword    ("bmstu");
    if (!m_db.open())
        qDebug() << ("Error: " + m_db.lastError().text());
}

MainWindow::MainWindow(QString login) : MainWindow()
{
    current_login = login;
    update_chats_list();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_chats_list()
{
    ui->chats_list->clear();
    QStringList list;
    QSqlQuery query("SELECT chat_name FROM pussy_chats, pussy_chats_link WHERE pussy_chats.chat_id = pussy_chats_link.chat_id AND user_login = '" + current_login + "'");
    while (query.next()) {
        list << query.value(0).toString();
    }
    ui->chats_list->addItems(list);
}

void MainWindow::on_chats_list_itemClicked(QListWidgetItem *item)
{
    ui->name_label->setText(item->text());
    //мемберы
    ui->members_list->clear();
    QStringList list;
    QSqlQuery query("SELECT user_login FROM pussy_chats, pussy_chats_link WHERE pussy_chats.chat_id = pussy_chats_link.chat_id AND chat_name = '" + item->text() + "'");
    while (query.next()) {
        list << query.value(0).toString();
    }
    ui->members_list->addItems(list);
}

void MainWindow::on_members_list_itemClicked(QListWidgetItem *item)
{
     ui->name_label->setText(item->text());
     update_chats_list();
}

void MainWindow::on_reconnection_button_triggered()
{
    close();
    initialization * init = new initialization;
    init->show();
}


void MainWindow::on_create_group_triggered()
{
    GroupDialog * gd = new GroupDialog;
    gd->show();
}
