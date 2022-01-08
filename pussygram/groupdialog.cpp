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

GroupDialog::GroupDialog(QString login, QString choice): GroupDialog(){
    m_login = login;
    m_choice = choice;
    if(m_choice == "Добавить друга"){
        ui->label->setText("Логин пользователя:");
        ui->buttonBox->setText("Добавить друга");
    }
    else if(m_choice == "Удалить друга"){
        ui->label->setText("Логин пользователя:");
        ui->buttonBox->setText("Удалить друга");
    }
}

GroupDialog::~GroupDialog()
{
    delete ui;
}


void GroupDialog::on_buttonBox_clicked()
{
    QString temp;
    QSqlQuery query(m_db);
    qDebug() << m_choice;
        if(m_choice == "Создать группу"){
            int chats_id;
            temp = "SELECT MAX(chat_id) FROM pussy_chats AS max;";
            query.exec(temp);
            query.first();
            chats_id = query.value(0).toInt();
            temp = "INSERT INTO pussy_chats VALUES(" +  QString::number(chats_id+1) + ",'" + m_login + "','true');";
            qDebug() << temp;
            if(query.exec(temp)){
                temp = "INSERT INTO pussy_chats_link VALUES (" +  QString::number(chats_id+1)+ ",'" + ui->group_edit->toPlainText() + "');";
                query.exec(temp);
                accept();
            }
        }
        else if(m_choice == "Добавить друга"){
            temp = "INSERT INTO pussy_friends VALUES('" + m_login + "','" + ui->group_edit->toPlainText() + "');";
            if(query.exec(temp)){
                qDebug() << query.exec(temp);
                temp = "INSERT INTO pussy_friends VALUES('" + ui->group_edit->toPlainText() + "','" + m_login + "');";
                query.exec(temp);
                accept();
            }
            else{
                ui->error_label->setText("Вы уже друзья");
            }
        }
        else if(m_choice == "Удалить друга"){
            temp = "DELETE FROM pussy_friends WHERE('" + m_login + "','" + ui->group_edit->toPlainText() + "');";
        }
}



