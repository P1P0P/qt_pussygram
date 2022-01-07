#include "mainwindow.h"
#include "initialization.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    initialization w;
    w.show();
    return a.exec();
}
