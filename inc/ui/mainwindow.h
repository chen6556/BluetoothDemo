#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "base/Bluetooth.hpp"
#include <QStringListModel>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void init();

private slots:
    void scan();
    void listDevs();
    void connectDev(const QModelIndex& index);
    void disconnectDev();
    void readBluetooth();
    void writeBluetooth();

    void connected();

private:
    Ui::MainWindow *ui;
    QStringListModel* _model;
    Bluetooth _bluetooth;
};
#endif // MAINWINDOW_H
