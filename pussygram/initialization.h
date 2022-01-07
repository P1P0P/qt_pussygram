#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class initialization;
}

class initialization : public QDialog
{
    Q_OBJECT

public:
    explicit initialization(QWidget *parent = nullptr);
    ~initialization();

private slots:

    void on_reg_button_clicked();

    void on_log_button_clicked();

    void on_login_box_currentIndexChanged(const QString &arg1);

    void on_reg2_button_clicked();

    void on_enter_button_clicked();

private:
    Ui::initialization *ui;
    QSqlDatabase local_db;
    QSqlDatabase m_db;
};

#endif // INITIALIZATION_H
