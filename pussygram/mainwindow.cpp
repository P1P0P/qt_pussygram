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

    ui->emoji_list->hide();
    ui->emoji_list->addItem("\U0001F638");
    ui->emoji_list->addItem("\U0001F639");
    ui->emoji_list->addItem("\U0001F63A");
    ui->emoji_list->addItem("\U0001F63B");
    ui->emoji_list->addItem("\U0001F63C");
    ui->emoji_list->addItem("\U0001F63D");
    ui->emoji_list->addItem("\U0001F63E");
    ui->emoji_list->addItem("\U0001F63F");
    ui->emoji_list->addItem("\U0001F431");
    ui->emoji_list->addItem("\U0001F436");
    ui->emoji_list->addItem("\U0001F640");
    ui->emoji_list->addItem("\U0001F351");
    ui->emoji_list->addItem("\U0001F381");
    ui->emoji_list->addItem("\U0001F4A3");
    ui->emoji_list->addItem("\U0001F339");
    ui->emoji_list->addItem("\U0001F36A");
    ui->emoji_list->addItem("\U0001F4A9");
    int count = ui->emoji_list->count();
    for(int i = 0; i < count; i++)
    {
      QListWidgetItem *item = ui->emoji_list->item(i);
      item->setSizeHint(QSize(50, 50));
    }
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
    set_item_size();
}

void MainWindow::on_chats_list_itemClicked(QListWidgetItem *item)
{
    ui->name_label->setText(item->text());
    //мемберы
    ui->members_list->clear();
    QStringList list;
    QSqlQuery query("SELECT nickname FROM pussy_chats JOIN pussy_chats_link \
                    ON pussy_chats.chat_id = pussy_chats_link.chat_id JOIN pussy_users\
                    ON user_login = login WHERE chat_name = '" + item->text() + "'");
    while (query.next()) {
        list << query.value(0).toString();
    }
    ui->members_list->addItems(list);
    show_chat(item);
    is_on_dialog = 0;
    set_item_size();
}

void MainWindow::on_members_list_itemClicked(QListWidgetItem *item)
{
     ui->name_label->setText(item->text());
     update_chats_list();
     is_on_dialog = 1;
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


    std::tuple<QString, QString, QString, QString> tuple;
    QString tmp = "SELECT nickname, message, date, sender_login FROM pussy_chats_link JOIN pussy_messages\
                   ON pussy_chats_link.chat_id = pussy_messages.chat_id JOIN pussy_users ON sender_login = login\
                   WHERE chat_name = '" + item->text() + "' ORDER BY 3";
    query.exec(tmp);
    while (query.next()){
        tuple = std::make_tuple(query.value(0).toString(), query.value(1).toString(), query.value(2).toString(), query.value(3).toString());

        if (std::get<3>(tuple) == current_login){
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

    std::tuple<QString, QString, QString, QString> tuple;
    QString tmp = "SELECT nickname, message, date, sender_login FROM pussy_dialog JOIN pussy_messages\
                   ON pussy_dialog.dialog_id = pussy_messages.chat_id JOIN pussy_users ON sender_login = login\
                   WHERE user1_login = '" + item->text() + "' ORDER BY 3";
    query.exec(tmp);
    while (query.next()){
        tuple = std::make_tuple(query.value(0).toString(),query.value(1).toString(),query.value(2).toString(), query.value(3).toString());

        if (std::get<3>(tuple) == current_login){
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
    if (is_on_dialog){
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

void MainWindow::set_item_size()
{
    //размер
    int count = ui->members_list->count();
    for(int i = 0; i < count; i++)
    {
      QListWidgetItem *item = ui->members_list->item(i);
      item->setSizeHint(QSize(item->sizeHint().width(), 50));
    }
    count = ui->chats_list->count();
    for(int i = 0; i < count; i++)
    {
      QListWidgetItem *item = ui->chats_list->item(i);
      item->setSizeHint(QSize(item->sizeHint().width(), 50));
    }
}


void MainWindow::on_emoji_button_clicked(bool checked)
{
    if (checked){
        ui->emoji_list->show();
    }
    else{
        ui->emoji_list->hide();
    }
}

