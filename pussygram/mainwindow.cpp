#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "initialization.h"
#include <QTimer>
#include <QScrollBar>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    showMaximized();

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
    ui->emoji_list->setIconSize(QSize(80,80));
    int count = ui->emoji_list->count();
    for(int i = 0; i < count; i++)
    {
      QListWidgetItem *item = ui->emoji_list->item(i);
      item->setSizeHint(QSize(80, 80));
    }
    time=new QTimer(this);
    time->setInterval(1000);
    QObject::connect(time,SIGNAL(timeout()),this,SLOT(refresh()));
    time->start();


    ui->chat_->verticalScrollBar()->setStyleSheet(QString::fromUtf8("QScrollBar:vertical {"
                                                                                           "    border: 1px solid #999999;"
                                                                                           "    background:white;"
                                                                                           "    width:10px;    "
                                                                                           "    margin: 0px 0px 0x 0px;"
                                                                                           "}"
                                                                                           "QScrollBar::handle:vertical {"
                                                                                           "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                                                                           "    stop: 0 rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130), stop:1 rgb(32, 47, 130));"
                                                                                           "    min-height: 0px;"
                                                                                           "}"
                                                                                           "QScrollBar::add-line:vertical {"
                                                                                           "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                                                                           "    stop: 0 rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130),  stop:1 rgb(32, 47, 130));"
                                                                                           "    height: 0px;"
                                                                                           "    subcontrol-position: bottom;"
                                                                                           "    subcontrol-origin: margin;"
                                                                                           "}"
                                                                                           "QScrollBar::sub-line:vertical {"
                                                                                           "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                                                                           "    stop: 0  rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130),  stop:1 rgb(32, 47, 130));"
                                                                                           "    height: 0 px;"
                                                                                           "    subcontrol-position: top;"
                                                                                           "    subcontrol-origin: margin;"
                                                                                           "}"
                                                                                           ));

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
    mode = 1;
    ui->kick->setEnabled(0);
    ui->input_line->clear();
    vector.clear();
    ui->name_label->setText(item->text());
    ui->members_list->clear();

    QStringList list;
    QSqlQuery query(m_db);
    query.exec("SELECT nickname, login, main_member FROM pussy_chats JOIN pussy_chats_link \
               ON pussy_chats.chat_id = pussy_chats_link.chat_id JOIN pussy_users\
               ON user_login = login WHERE chat_name = '" + item->text() + "' AND login != '" + current_login + "'");
    while (query.next()){
        list << query.value(0).toString();
    }
    ui->members_list->addItems(list);

    query.exec("SELECT main_member FROM pussy_chats JOIN pussy_chats_link \
               ON pussy_chats.chat_id = pussy_chats_link.chat_id JOIN pussy_users\
               ON user_login = login WHERE chat_name = '" + item->text() + "' AND login = '" + current_login + "'");
    query.first();
    if (query.value(0).toBool() == true){
        ui->kick->setEnabled(1);
    }

    if (item->text() != "Users"){
        show_chat(item);
        ui->input_line->setEnabled(1);
        ui->input_line->setFocus();
    }
    else{
        ui->input_line->setEnabled(0);
        ui->chat_->clear();
    }
    set_item_size();

}

void MainWindow::on_members_list_itemClicked(QListWidgetItem *item)
{

     QSqlQuery query(m_db);
     query.exec("SELECT login FROM pussy_users WHERE nickname = '" + item->text() + "'");
     query.first();
     if (query.value(0).toString() != current_login){
         mode = 2;
         ui->name_label->setText(item->text());
         update_chats_list();
         show_dialog(item);
         ui->input_line->setEnabled(1);
         vector.clear();
         ui->kick->setEnabled(0);
         ui->input_line->setFocus();
     }
     else
         on_reconnection_button_triggered();
}

void MainWindow::on_reconnection_button_triggered()
{
    close();
    initialization * init = new initialization;
    init->show();
}


void MainWindow::on_create_group_triggered()
{
    GroupDialog * gd = new GroupDialog(current_login, ui->create_group->text(), "");
    if (gd->exec() == QDialog::Accepted)
        update_chats_list();
}


void MainWindow::on_enter_group_triggered()
{
    GroupDialog * gd = new GroupDialog(current_login, ui->enter_group->text(), "");
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
    last_message_id = 0;
    last_sender = "";
    query.exec("SELECT nickname, message, date, sender_login, message_id FROM pussy_chats_link JOIN pussy_messages\
               ON pussy_chats_link.chat_id = pussy_messages.chat_id JOIN pussy_users\
               ON sender_login = login WHERE chat_name = '" + item->text() + "' ORDER BY 3");
    while (query.next()){
        show_message(query.value(0), query.value(1), query.value(2), query.value(3), query.value(4));
    }
    query.last();
    last_sender = query.value(0).toString();
    last_message_id = query.value(4).toInt();
}

void MainWindow::show_dialog(QListWidgetItem *item)
{
    ui->chat_->clear();
    QFont font;
    font.setPointSize(12);
    ui->chat_->setFont(font);
    QSqlQuery query(m_db);
    int dialog_id = 0;
    last_message_id = 0;
    last_sender = "";
    query.exec("SELECT login FROM pussy_users WHERE nickname = '" + item->text() + "'");
    query.first();
    QString login = query.value(0).toString();

    query.exec("SELECT dialog_id FROM pussy_dialog WHERE user1_login = '" + login + "' AND user2_login = '" + current_login + "'");
    if (!query.next()){
        query.exec("SELECT MAX(chat_id) FROM (\
                   (SELECT MAX(chat_id) AS chat_id FROM pussy_chats)\
                   union\
                   (SELECT MAX(dialog_id) AS chat_id FROM pussy_dialog)\
                   ) COMBINED");
        query.first();
        dialog_id = query.value(0).toInt();
        query.exec("INSERT INTO pussy_dialog VALUES(" +  QString::number(dialog_id + 1) + ", '" + current_login + "', '" + login + "'), \
                    (" +  QString::number(dialog_id + 1) + ", '" + login + "', '" + current_login + "')");
    }

    QString tmp = "SELECT nickname, message, date, sender_login, message_id FROM pussy_dialog JOIN pussy_messages\
                   ON pussy_dialog.dialog_id = pussy_messages.chat_id JOIN pussy_users\
                   ON sender_login = login WHERE user1_login = '" + login + "' AND user2_login = '" + current_login + "' ORDER BY 3";
    query.exec(tmp);

    while (query.next()){
        show_message(query.value(0), query.value(1), query.value(2), query.value(3), query.value(4));
    }
    query.last();
    last_sender = query.value(0).toString();
    last_message_id = query.value(4).toInt();
}

void MainWindow::show_message(QVariant val0, QVariant val1, QVariant val2, QVariant val3, QVariant val4)
{
    int prev_pos = 0;
    QSqlQuery query2(m_db);

    if (val3.toString() == current_login){
        ui->chat_->setAlignment(Qt::AlignRight);
        if (last_sender != val3.toString()){
            ui->chat_->insertHtml("<div align=\"right\"><font color=\"DeepSkyBlue\">" + val0.toString() + "</font></div>");
            ui->chat_->setTextColor("white");
        }
    }
    else{
        ui->chat_->setAlignment(Qt::AlignLeft);
        if (last_sender != val3.toString()){
            ui->chat_->insertHtml("<div align=\"left\"><font color=\"DeepSkyBlue\">" + val0.toString() + "</font></div>");
            ui->chat_->setTextColor("white");
        }
    }

    ui->chat_->append("");
    last_sender = val3.toString();

    query2.exec("SELECT position, image_id FROM pussy_images WHERE message_id = " + QString::number(val4.toInt()));

    QString tmp = decrypt(val1.toString());

    while (query2.next())
    {
        ui->chat_->insertPlainText(tmp.left(query2.value(0).toInt() - prev_pos));
        ui->chat_->insertHtml(QString("<img src=\"%1\" width=\"70\" height=\"70\">").arg(("./img/" + query2.value(1).toString() + ".png")));
        tmp = tmp.remove(0, query2.value(0).toInt() - prev_pos);
        prev_pos = query2.value(0).toInt() + 1;
    }
    ui->chat_->insertPlainText(tmp);

    ui->chat_->append(val2.toDateTime().toString("ddd hh:mm"));
    ui->chat_->append("");
    ui->chat_->verticalScrollBar()->setValue(ui->chat_->verticalScrollBar()->maximum());
}

void MainWindow::on_send_button_clicked()
{
    if (ui->input_line->toPlainText().length() != 0){
        QSqlQuery query(m_db);
        int chat_id;
        QString temp;

        query.exec("SELECT MAX(message_id) FROM pussy_messages");
        query.first();
        QString message_id = QString::number(query.value(0).toInt() + 1);
        if (mode == 2){
            query.exec("SELECT login FROM pussy_users WHERE nickname = '" + ui->members_list->currentItem()->text() + "'");
            query.first();
            QString login = query.value(0).toString();

            temp = "SELECT dialog_id FROM pussy_dialog WHERE user1_login = '" + login
                    + "' AND user2_login = '" + current_login + "'";
            query.exec(temp);
            query.first();
            chat_id = query.value(0).toInt();
            QDateTime cur_time = QDateTime::currentDateTime();
            QString current_time = cur_time.toString("yyyy-MM-dd hh:mm:ss");
            temp = "INSERT INTO pussy_messages VALUES(" + message_id + ", " + QString::number(chat_id) + ",'" + current_login + "','"
                    + encrypt(ui->input_line->toPlainText()) + "','" + current_time + "');";

            query.exec(temp);
            while (!vector.empty()){
                query.exec("INSERT INTO pussy_images VALUES (" + message_id + ", " + QString::number(vector[0].first) + ", " + QString::number(vector[0].second) + ")");
                vector.erase(vector.begin());
            }
            show_dialog(ui->members_list->currentItem());
            ui->input_line->clear();
        }
        else if (mode == 1){
            temp = "SELECT chat_id FROM pussy_chats_link WHERE chat_name = '" + ui->chats_list->currentItem()->text()+ "';";
            query.exec(temp);
            query.first();
            chat_id = query.value(0).toInt();
            QDateTime cur_time = QDateTime::currentDateTime();
            QString current_time = cur_time.toString("yyyy-MM-dd hh:mm:ss");
            temp = "INSERT INTO pussy_messages VALUES(" + message_id + ", " + QString::number(chat_id) + ",'" + current_login + "','"
                    + encrypt(ui->input_line->toPlainText()) + "','" + current_time + "');";

            query.exec(temp);
            while (!vector.empty()){
                query.exec("INSERT INTO pussy_images VALUES (" + message_id + ", " + QString::number(vector[0].first) + ", " + QString::number(vector[0].second) + ")");
                vector.erase(vector.begin());
            }
            show_chat(ui->chats_list->currentItem());
            ui->input_line->clear();
        }
        ui->input_line->setFocus();
        ui->chat_->verticalScrollBar()->setValue(ui->chat_->verticalScrollBar()->maximum());
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

void MainWindow::refresh()
{
    int pos = ui->chat_->verticalScrollBar()->value();
    QSqlQuery query(m_db);
    if (mode == 2){
        query.exec("SELECT nickname, message, date, sender_login, message_id FROM pussy_dialog JOIN pussy_messages\
                   ON pussy_dialog.dialog_id = pussy_messages.chat_id JOIN pussy_users\
                   ON sender_login = login WHERE user1_login = (SELECT login FROM pussy_users WHERE nickname = '" + ui->members_list->currentItem()->text() + "') \
                   AND user2_login = '" + current_login + "' ORDER BY 3");
        query.last();
        if (last_message_id != query.value(4).toInt()){
                show_message(query.value(0), query.value(1), query.value(2), query.value(3), query.value(4));
                last_sender = query.value(0).toString();
        }
    }
    else if (mode == 1){
        query.exec("SELECT nickname, message, date, sender_login, message_id  FROM pussy_messages JOIN pussy_chats_link\
                     ON pussy_messages.chat_id = pussy_chats_link.chat_id JOIN pussy_users\
                     ON login = sender_login WHERE chat_name = '" + ui->chats_list->currentItem()->text() + "' ORDER BY 3");
        query.last();
        if (last_message_id != query.value(4).toInt()){
                show_message(query.value(0), query.value(1), query.value(2), query.value(3), query.value(4));
                last_sender = query.value(0).toString();
        }
    }

    ui->chat_->verticalScrollBar()->setValue(pos);
}

void MainWindow::on_exit_group_triggered()
{
    GroupDialog * gd = new GroupDialog(current_login, ui->exit_group->text(), "");
    if (gd->exec() == QDialog::Accepted){
        update_chats_list();
        ui->members_list->clear();
        ui->chat_->clear();
        ui->name_label->setText("Выберите группу");
        mode = 0;
        ui->input_line->setEnabled(0);
    }
}

void MainWindow::on_delete_account_triggered()
{
    GroupDialog * gd = new GroupDialog(current_login, ui->delete_account->text(), "");
    if (gd->exec() == QDialog::Accepted)
        on_reconnection_button_triggered();
}

void MainWindow::on_kick_triggered()
{
    GroupDialog * gd = new GroupDialog(current_login, ui->kick->text(), ui->chats_list->currentItem()->text());
    if (gd->exec() == QDialog::Accepted){
        on_chats_list_itemClicked(ui->chats_list->currentItem());
    }
}

void MainWindow::on_emoji_list_itemClicked(QListWidgetItem *item)
{
    if (mode != 0){
        vector.push_back(std::make_pair(ui->input_line->textCursor().position(), ui->emoji_list->currentRow() + 1));
        ui->input_line->insertHtml(QString("<img src=\"%1\" width=\"70\" height=\"70\">").arg(("./img/" + QString(std::to_string(ui->emoji_list->currentRow() + 1).c_str()) + ".png")));
    }
    ui->input_line->setFocus();
}

void MainWindow::on_input_line_textChanged()
{
    int pos = 0;
    int len = 0;
    if (before.size() > ui->input_line->toPlainText().size()){
        for (int i = 0; i < before.size(); i++){
            if (before[i] != ui->input_line->toPlainText()[i]){
                pos = i;
                len = before.size() - ui->input_line->toPlainText().size();
                break;
            }
        }
        for (int i = 0; i < vector.size(); i++){
            if (pos < vector[i].first){
                if (vector[i].first - pos >= len){
                    vector[i].first = vector[i].first - len;
                }
                else{
                    vector.erase(vector.begin() + i);
                    i--;
                }
            }
        }
    }
    else{
        for (int i = 0; i < before.size(); i++){
            if (before[i] != ui->input_line->toPlainText()[i]){
                pos = i;
                len = ui->input_line->toPlainText().size() - before.size();
                break;
            }
        }
        for (int i = 0; i < vector.size(); i++){
            if (pos <= vector[i].first){
                vector[i].first = vector[i].first + len;
            }
        }
    }

    before = ui->input_line->toPlainText();
}


QString MainWindow::encrypt(QString input)
{
    QString alphabet = "Лрв>Дn9JiЖО\":жР,зHTsЙПhjмЭАb!.yGпЯкmf2Ц Y0УЧВKоgcКIFСQM5l4ёUМщЫLИeЕ?NТ/#RvЬНpБ71ГЮDэBФШEЩk%З=Zo-'сюV^<ХъOеSXЪля@qа)aWC+*шrун;йцдчtт_P&(`фw$ыхdи~A63бёгu|ь8zx";
    for (int i = 0; i < input.size(); i++)
    {
        if (input[i] != "￼")
            input.replace(i, 1, alphabet[(alphabet.indexOf(input[i]) + 3) % 157]);
        else{
            input.remove(i, 1);
            i--;
        }
    }
    return input;
}

QString MainWindow::decrypt(QString input)
{
    QString alphabet = "Лрв>Дn9JiЖО\":жР,зHTsЙПhjмЭАb!.yGпЯкmf2Ц Y0УЧВKоgcКIFСQM5l4ёUМщЫLИeЕ?NТ/#RvЬНpБ71ГЮDэBФШEЩk%З=Zo-'сюV^<ХъOеSXЪля@qа)aWC+*шrун;йцдчtт_P&(`фw$ыхdи~A63бёгu|ь8zx";
    for (int i = 0; i < input.size(); i++){
        if (alphabet.contains(input[i]))
            input.replace(i, 1, alphabet[(alphabet.indexOf(input[i]) - 3 + 157) % 157]);
    }
    return input;
}

void MainWindow::on_exit_triggered()
{
    QApplication::quit();
}
