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

GroupDialog::~GroupDialog()
{
    delete ui;
}


void GroupDialog::on_buttonBox_clicked()
{
    QSqlQuery query(m_db);
    if(query.exec("INSERT INTO pussy_chats VALUES()")){
        accept();
    }
}



