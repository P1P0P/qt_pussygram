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

private:
    QString current_login;
    Ui::MainWindow *ui;
    QSqlDatabase m_db;
    void update_chats_list();
};
#endif // MAINWINDOW_H
