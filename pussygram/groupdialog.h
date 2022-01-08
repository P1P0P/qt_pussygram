#ifndef GROUPDIALOG_H
#define GROUPDIALOG_H

#include <QDialog>
#include <QtSql>
#include <mainwindow.h>

namespace Ui {
class GroupDialog;
}

class GroupDialog : public QDialog
{
    Q_OBJECT

public:
     GroupDialog(QWidget *parent = nullptr);
     GroupDialog(QString, QString);
    ~GroupDialog();

private slots:

    void on_buttonBox_clicked();

private:
    Ui::GroupDialog *ui;
    QSqlDatabase m_db;
    QString m_login;
    QString m_choice;
};

#endif // GROUPDIALOG_H
