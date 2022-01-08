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
    show_chat(item);
}

void MainWindow::on_members_list_itemClicked(QListWidgetItem *item)
{
     ui->name_label->setText(item->text());
     update_chats_list();
     show_dialog(item);
}

void MainWindow::on_reconnection_button_triggered()
{
    close();
    initialization * init = new initialization;
    init->show();
}


void MainWindow::on_create_group_triggered()
{
    GroupDialog * gd = new GroupDialog(current_login, ui->create_group->text());
    if (gd->exec() == QDialog::Accepted)
        update_chats_list();
}


void MainWindow::on_enter_group_triggered()
{
    GroupDialog * gd = new GroupDialog(current_login, ui->enter_group->text());
    if (gd->exec() == QDialog::Accepted)
        update_chats_list();
}

void MainWindow::show_chat(QListWidgetItem *item)
{
    ui->chat_->clear();
    QFont font;
    font.setPointSize(12);
    ui->chat_->setFont(font);
    QSqlQuery query(m_db);


    std::tuple<QString, QString, QString> tuple;
    QString tmp = "SELECT nickname, message, date FROM pussy_chats_link JOIN pussy_messages\
                   ON pussy_chats_link.chat_id = pussy_messages.chat_id JOIN pussy_users ON sender_login = login\
                   WHERE chat_name = '" + item->text() + "'";
    query.exec(tmp);
    while (query.next()){
        tuple = std::make_tuple(query.value(0).toString(),query.value(1).toString(),query.value(2).toString());

        if (std::get<0>(tuple) == current_login){
            ui->chat_->setAlignment(Qt::AlignRight);
        }
        else{
            ui->chat_->setAlignment(Qt::AlignLeft);
        }

        ui->chat_->append(std::get<0>(tuple));
        ui->chat_->append(std::get<1>(tuple));
        ui->chat_->append(std::get<2>(tuple));
        ui->chat_->append("");
    }
}

void MainWindow::show_dialog(QListWidgetItem *item)
{
    ui->chat_->clear();
    QFont font;
    font.setPointSize(12);
    ui->chat_->setFont(font);
    QSqlQuery query(m_db);
    int dialog_id;

    query.exec("SELECT dialog_id FROM pussy_dialog WHERE user1_login = '" + item->text() + "' AND user2_login = '" + current_login + "'");
    if (!query.next()){
        query.exec("SELECT MAX(chat_id) FROM (\
                   (SELECT MAX(chat_id) AS chat_id FROM pussy_chats)\
                   union\
                   (SELECT MAX(dialog_id) AS chat_id FROM pussy_dialog)\
                   ) COMBINED");
        query.first();
        dialog_id = query.value(0).toInt();
        qDebug() << dialog_id;
        query.exec("INSERT INTO pussy_dialog VALUES(" +  QString::number(dialog_id + 1) + ", '" + current_login + "', '" + item->text() + "'), \
                    (" +  QString::number(dialog_id + 1) + ", '" + item->text() + "', '" + current_login + "')");
    }

    std::tuple<QString, QString, QString> tuple;
    QString tmp = "SELECT nickname, message, date FROM pussy_dialog JOIN pussy_messages\
                   ON pussy_dialog.dialog_id = pussy_messages.chat_id JOIN pussy_users ON sender_login = login\
                   WHERE user1_login = '" + item->text() + "'";
    query.exec(tmp);
    while (query.next()){
        tuple = std::make_tuple(query.value(0).toString(),query.value(1).toString(),query.value(2).toString());

        if (std::get<0>(tuple) == current_login){
            ui->chat_->setAlignment(Qt::AlignRight);
        }
        else{
            ui->chat_->setAlignment(Qt::AlignLeft);
        }

        ui->chat_->append(std::get<0>(tuple));
        ui->chat_->append(std::get<1>(tuple));
        ui->chat_->append(std::get<2>(tuple));
        ui->chat_->append("");
    }
}

void MainWindow::on_send_button_clicked()
{
    QSqlQuery query(m_db);
    int chat_id;
    QString temp;
    if (ui->members_list->isItemSelected(ui->members_list->currentItem())){
        temp = "SELECT dialog_id FROM pussy_dialog WHERE user1_login = '" + ui->members_list->currentItem()->text() + "';";
        qDebug() << temp;
        query.exec(temp);
        query.first();
        chat_id = query.value(0).toInt();
        QDateTime cur_time = QDateTime::currentDateTime();
        QString current_time = cur_time.toString("yyyy-MM-dd hh:mm:ss");
        temp = "INSERT INTO pussy_messages VALUES(" + QString::number(chat_id) + ",'" + current_login + "','"
                + ui->input_line->toPlainText() + "','" + current_time + "');";
        ui->input_line->clear();
        qDebug() << temp;
        query.exec(temp);
        show_dialog(ui->members_list->currentItem());
    }
    else{
        temp = "SELECT chat_id FROM pussy_chats_link WHERE chat_name = '" + ui->chats_list->currentItem()->text()+ "';";
        qDebug() << temp;
        query.exec(temp);
        query.first();
        chat_id = query.value(0).toInt();
        QDateTime cur_time = QDateTime::currentDateTime();
        QString current_time = cur_time.toString("yyyy-MM-dd hh:mm:ss");
        temp = "INSERT INTO pussy_messages VALUES(" + QString::number(chat_id) + ",'" + current_login + "','"
                + ui->input_line->toPlainText() + "','" + current_time + "');";
        ui->input_line->clear();
        qDebug() << temp;
        query.exec(temp);
        show_chat(ui->chats_list->currentItem());
    }
}

