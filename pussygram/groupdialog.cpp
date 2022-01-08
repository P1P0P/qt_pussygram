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
    int chats_id;
    qDebug() << m_choice;
        if(m_choice == "Создать группу"){
            query.exec("SELECT MAX(chat_id) FROM (\
                       (SELECT MAX(chat_id) AS chat_id FROM pussy_chats)\
                       union\
                       (SELECT MAX(dialog_id) AS chat_id FROM pussy_dialog)\
                       ) COMBINED");
            query.first();
            chats_id = query.value(0).toInt();
            temp = "INSERT INTO pussy_chats VALUES(" + QString::number(chats_id+1) + ",'" + m_login + "','true');";
            qDebug() << temp;
            if(query.exec(temp)){
                temp = "INSERT INTO pussy_chats_link VALUES (" + QString::number(chats_id+1)+ ",'" + ui->group_edit->toPlainText() + "');";
                query.exec(temp);
                accept();
            }
        }
        else{
            QString chat_name;
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
}



