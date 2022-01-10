#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QListWidget>
#include <groupdialog.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    MainWindow(QString);

    ~MainWindow();

private slots:
    void on_chats_list_itemClicked(QListWidgetItem *item);

    void on_members_list_itemClicked(QListWidgetItem *item);

    void on_reconnection_button_triggered();

    void on_create_group_triggered();

    void on_enter_group_triggered();

    void on_exit_group_triggered();

    void on_send_button_clicked();

    void set_item_size();

    void on_emoji_button_clicked(bool checked);

    void refresh();

    void on_delete_account_triggered();

    void on_exit_triggered();

    void on_kick_triggered();

private:
    QString current_login;
    Ui::MainWindow *ui;
    QSqlDatabase m_db;
    void update_chats_list();
    void show_chat(QListWidgetItem *item);
    void show_dialog(QListWidgetItem *item);

    int mode = 0; //none - 0, chat - 1, dialog - 2
    QTimer *time;
};
#endif // MAINWINDOW_H
