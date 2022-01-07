#include "initialization.h"
#include "ui_initialization.h"
#include "mainwindow.h"

initialization::initialization(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::initialization)
{
    ui->setupUi(this);

    ui->sign_up_widget->hide();
    setFixedSize(500, 300);

    //логины
    local_db = QSqlDatabase::addDatabase("QSQLITE");
    local_db.setDatabaseName("db_password");
    local_db.open();

    QSqlQuery query(local_db);
    query.exec("CREATE TABLE passwords(login TEXT PRIMARY KEY, password TEXT)");

    QStringList list;
    query.exec("SELECT login FROM passwords");
    while (query.next()){
        qDebug() << query.value(0).toString();
        list << query.value(0).toString();
    }
    ui->login_box->addItems(list);
}

initialization::~initialization()
{
    delete ui;
}


void initialization::on_reg_button_clicked()
{
    ui->sign_in_widget->hide();
    ui->sign_up_widget->show();
    ui->label_4->setText("Уже есть аккаунт?");
}


void initialization::on_log_button_clicked()
{
    ui->sign_up_widget->hide();
    ui->sign_in_widget->show();
}


void initialization::on_login_box_currentIndexChanged(const QString &arg1)
{
    QSqlQuery query(local_db);
    query.exec("SELECT password FROM passwords WHERE login = '" + arg1 + "'");
    query.next();
    ui->password_line->setText(query.value(0).toString());
}


void initialization::on_enter_button_clicked()
{
    MainWindow *main;
    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setDatabaseName("fn1131_2021");
    m_db.setHostName    ("195.19.32.74");
    m_db.setPort        (5432);
    m_db.setUserName    ("student");
    m_db.setPassword    ("bmstu");
    if (!m_db.open())
        qDebug() << ("Error: " + m_db.lastError().text());

    QSqlQuery query1(m_db);
    std::string temp = "SELECT login,password FROM pussy_users WHERE login ='"
            + ui->login_box->currentText().toStdString() + "' and password ='"
            + ui->password_line->text().toStdString() + "';";
     query1.exec(temp.c_str());
     if (query1.next()){
         close();
         main = new MainWindow();
         main->show();
     }
     else{
         ui->error_label->setText("Ошибка");
     }
}



void initialization::on_reg2_button_clicked()
{

    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setDatabaseName("fn1131_2021");
    m_db.setHostName    ("195.19.32.74");
    m_db.setPort        (5432);
    m_db.setUserName    ("student");
    m_db.setPassword    ("bmstu");
    if (!m_db.open())
        qDebug() << ("Error: " + m_db.lastError().text());

    ui->label_4->setText("");
        QSqlQuery query(m_db);
        std::string temp = "SELECT login FROM pussy_users WHERE login ='" + ui->login_line->text().toStdString() + "';";
        query.exec(temp.c_str());
        if (!query.next())
        {
            temp = "INSERT INTO pussy_users VALUES('" + ui->login_line->text().toStdString()
                    + "','" + ui->password_line_3->text().toStdString()
                    + "','" + ui->nickname_line->text().toStdString() + "');";
            qDebug() << temp.c_str();
            qDebug() << query.exec(temp.c_str());
            qDebug() << query.lastError();
            ui->label_4->setText("Регистрация прошла успешна");
        }
}

