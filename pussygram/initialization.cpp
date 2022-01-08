#include "initialization.h"
#include "ui_initialization.h"
#include "mainwindow.h"

initialization::initialization(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::initialization)
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

    ui->sign_up_widget->hide();
    setFixedSize(500, 300);

    //логины
    local_db = QSqlDatabase::addDatabase("QSQLITE", "local_db");
    local_db.setDatabaseName("db_password");
    local_db.open();

    QSqlQuery query(local_db);
    query.exec("CREATE TABLE passwords(login TEXT PRIMARY KEY, password TEXT)");

    QStringList list;
    query.exec("SELECT login FROM passwords");
    while (query.next()){
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


void initialization::on_enter_button_clicked() //вход
{

    QSqlQuery query(m_db);
    QString temp = "SELECT login,password FROM pussy_users WHERE login ='"
            + ui->login_box->currentText() + "' and password ='"
            + ui->password_line->text() + "';";
    query.exec(temp);
    if (query.next()){
        if (ui->remember_button->isChecked()){
            QSqlQuery query2(local_db);
            temp = "INSERT INTO passwords VALUES('" + ui->login_box->currentText() + "','" + ui->password_line->text() + "')";
            query2.exec(temp);
        }
        close();
        MainWindow *mainWindow;
        mainWindow = new MainWindow(ui->login_box->currentText());
        mainWindow->show();
    }
    else{
        ui->error_label->setText("Ошибка");
    }
}



void initialization::on_reg2_button_clicked() //регистрация
{
    ui->label_4->setText("");
    QSqlQuery query(m_db);
    QString temp = "SELECT login FROM pussy_users WHERE login ='" + ui->login_line->text() + "';";
    query.exec(temp);
        if (!query.next()){
            temp = "INSERT INTO pussy_users VALUES('" + ui->login_line->text()
                    + "','" + ui->password_line_3->text()
                    + "','" + ui->nickname_line->text() + "');";
            query.exec(temp);
            temp = "INSERT INTO pussy_chats VALUES(1,'" + ui->login_line->text() + "','false');";
            query.exec(temp);
            ui->label_4->setText("Регистрация прошла успешна");
        }
}

