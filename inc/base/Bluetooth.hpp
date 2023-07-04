#pragma once
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QBluetoothSocket>
#include <QBluetoothServer>
#include <QTimer>

class Bluetooth : public QObject
{
    Q_OBJECT
private:
    QBluetoothDeviceDiscoveryAgent _detector;
    QBluetoothLocalDevice *_local = nullptr;
    QBluetoothSocket *_socket = nullptr;
    QBluetoothServiceInfo _server_info;
    QBluetoothServer _server;
    QList<QBluetoothDeviceInfo> _devices;
    QTimer _timer;

    QByteArray _cache;

public:
    Bluetooth();
    ~Bluetooth();

    const QByteArray &cache() const;
    const QBluetoothDeviceDiscoveryAgent *device_discovery_agent() const;
    const QBluetoothLocalDevice *local_device() const;
    const QBluetoothSocket *socket() const;
    const QTimer *timer() const;
    const QList<QBluetoothDeviceInfo> &devices() const;

public:
signals:
    void refreshed();
    void read_ready();
    void connected();
    void disconnected();

private slots:
    void refresh_devices();
    void reset_socket();
    void new_connection();
    void catch_error(const QBluetoothSocket::SocketError error);

public slots:
    void scan();
    void connect_to_address(const QBluetoothAddress &address);
    void disconnect_from_address();
    void read();
    void write(const QString &msg);
};