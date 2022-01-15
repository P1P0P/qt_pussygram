#include "groupdialog.h"
#include "ui_groupdialog.h"


GroupDialog::GroupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GroupDialog)
{
    ui->setupUi(this);

    m_db = QSqlDatabase::addDatabase("QPSQL", "PSQL");
    m_db.setDatabaseName("fn1131_2021");
    m_db.setHostName    ("195.19.32.74");
    m_db.setPort        (5432);
    m_db.setUserName    ("student");
    m_db.setPassword    ("bmstu");
    if (!m_db.open())
        qDebug() << ("Error: " + m_db.lastError().text());
}

GroupDialog::GroupDialog(QString login, QString choice, QString cur_chat): GroupDialog(){
    m_login = login;
    m_choice = choice;
    m_cur_chat = cur_chat;
    if(m_choice == "Войти в группу"){
        ui->buttonBox->setText("Войти в группу");
    }
    else if(m_choice == "Выйти из группы"){
        ui->buttonBox->setText("Выйти из группы");
    }
    else if (m_choice == "Удалить аккаунт"){
        ui->group_edit->hide();
        ui->label->setText("Вы уверены, что хотите удалить аккаунт?");
        ui->label->setGeometry(80,100,300,30);
        ui->buttonBox->setText("Да, удалить");
    }
    else if (m_choice == "Выгнать из группы"){
        ui->label->setText("Кого Вы хотите выгнать?");
        ui->buttonBox->setText("Выгнать");
    }
    //setWindowFlags(Qt::FramelessWindowHint);
}

GroupDialog::~GroupDialog()
{
    delete ui;
}


void GroupDialog::on_buttonBox_clicked()
{
    QString temp;
    QSqlQuery query(m_db);
    int chats_id;
    if(m_choice == "Создать группу"){
        query.exec("SELECT MAX(chat_id) FROM (\
                   (SELECT MAX(chat_id) AS chat_id FROM pussy_chats_link)\
                   union\
                   (SELECT MAX(dialog_id) AS chat_id FROM pussy_dialog)\
                   ) COMBINED");
        query.first();
        chats_id = query.value(0).toInt();
        temp = "INSERT INTO pussy_chats_link VALUES (" + QString::number(chats_id+1)+ ",'" + ui->group_edit->toPlainText() + "');";
        if(query.exec(temp)){
            temp = "INSERT INTO pussy_chats VALUES(" + QString::number(chats_id+1) + ",'" + m_login + "','true');";
            query.exec(temp);
            accept();
        }
        else
            ui->error_label->setText("Группа с таким названием уже существует");
    }
    else if (m_choice == "Выйти из группы"){
        temp = "SELECT chat_id FROM pussy_chats_link WHERE chat_name = '" + ui->group_edit->toPlainText() + "';";
        query.exec(temp);
        query.first();
        chats_id = query.value(0).toInt();
        temp = "DELETE FROM pussy_chats WHERE chat_id =" + QString::number(chats_id)+ " and user_login = '" + m_login + "';";
        if(query.exec(temp) && chats_id != 0)
            accept();
        else
            ui->error_label->setText("Такой группы не существует");
    }
    else if (m_choice == "Удалить аккаунт"){
        temp = "DELETE FROM pussy_users WHERE login = '" + m_login + "';";
        query.exec(temp);
        accept();
    }
    else if (m_choice == "Войти в группу"){
        temp = "SELECT chat_id FROM pussy_chats_link WHERE chat_name = '" + ui->group_edit->toPlainText() + "';";
        query.exec(temp);
        query.first();
        chats_id = query.value(0).toInt();
        if(chats_id != 0){
            temp = "INSERT INTO pussy_chats VALUES(" +  QString::number(chats_id) + ",'" + m_login + "','false');";
            query.exec(temp);
            accept();
        }
        else
            ui->error_label->setText("Такой группы не существует");
    }
    else{
        query.exec("SELECT chat_id FROM pussy_chats_link WHERE chat_name = '" + m_cur_chat + "'");
        query.first();
        int chat_id = query.value(0).toInt();
        query.exec("DELETE FROM pussy_chats WHERE chat_id = " + QString::number(chat_id) + " AND user_login = '"
                   + ui->group_edit->toPlainText() + "'");
        accept();
    }
}

void GroupDialog::on_pushButton_clicked()
{
    close();
}

