#include "ui/mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    _model = new QStringListModel();
    ui->listView->setModel(_model);
    QObject::connect(&_bluetooth, &Bluetooth::refreshed, this, &MainWindow::listDevs);
    QObject::connect(&_bluetooth, &Bluetooth::read_ready, this, &MainWindow::readBluetooth);
}

void MainWindow::scan()
{
    _bluetooth.scan();
}

void MainWindow::listDevs()
{
    ui->textBrowser->clear();
    ui->textEdit->clear();
    QStringList list;
    for (const QBluetoothDeviceInfo& info: _bluetooth.devices())
    {
        list.append(info.name());
    }
    _model->setStringList(list);
}

void MainWindow::connectDev(const QModelIndex& index)
{
    const QList<QBluetoothDeviceInfo> &devices = _bluetooth.devices();
    const QBluetoothDeviceInfo& info = *std::find_if(devices.cbegin(), devices.cend(), [&](const QBluetoothDeviceInfo& f){return f.name() == index.data().toString();});
    _bluetooth.connect_to_address(info.address());
}

void MainWindow::disconnectDev()
{
    _bluetooth.disconnect_from_address();
}

void MainWindow::readBluetooth()
{
    ui->textBrowser->setPlainText(QString(_bluetooth.cache()));
}

void MainWindow::writeBluetooth()
{
    _bluetooth.write(QByteArray(ui->textEdit->toPlainText().toStdString().c_str()));
    ui->textEdit->clear();
}