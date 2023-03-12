#include "ui/mainwindow.h"

#include <QStyleFactory>
#include <QApplication>
#include "base/Bluetooth.hpp"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    w.show();
    
    return a.exec();
}
