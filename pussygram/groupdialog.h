#ifndef GROUPDIALOG_H
#define GROUPDIALOG_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class GroupDialog;
}

class GroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GroupDialog(QWidget *parent = nullptr);
    ~GroupDialog();
    QString get_group_name();


private slots:

    void on_buttonBox_clicked();

private:
    Ui::GroupDialog *ui;
    QSqlDatabase m_db;

};

#endif // GROUPDIALOG_H
