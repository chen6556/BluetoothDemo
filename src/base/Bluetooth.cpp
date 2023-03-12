#include "base/Bluetooth.hpp"

Bluetooth::Bluetooth()
{
    _local = new QBluetoothLocalDevice();
    _socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    QObject::connect(&_timer, &QTimer::timeout, &_detector, &QBluetoothDeviceDiscoveryAgent::stop);
    QObject::connect(&_detector, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &Bluetooth::refresh_devices);
    QObject::connect(_local, &QBluetoothLocalDevice::deviceConnected, this, &Bluetooth::connect_to_address);
    QObject::connect(_socket, &QBluetoothSocket::disconnected, this, &Bluetooth::reset_socket);
    QObject::connect(_socket, &QBluetoothSocket::readyRead, this, &Bluetooth::read);
}

Bluetooth::~Bluetooth()
{
    if (_local != nullptr)
    {
        delete _local;
    }
    if (_socket != nullptr)
    {
        delete _socket;
    }
}

void Bluetooth::refresh_devices()
{
    _devices = _detector.discoveredDevices();
    emit refreshed();
}

void Bluetooth::scan()
{
    _detector.start();
    _timer.start(10000);
}

void Bluetooth::connect_to_address(const QBluetoothAddress& address)
{
    _socket->connectToService(address, QBluetoothUuid("00001101-0000-1000-8000-00805F9B34FB"));
    emit connected();
}

void Bluetooth::disconnect_from_address()
{
    _socket->disconnectFromService();
    emit disconnected();
}

void Bluetooth::reset_socket()
{
    delete _socket;
    _socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
}

void Bluetooth::read()
{
    _cache = _socket->readAll();
    emit read_ready();
}

void Bluetooth::write(const QString& msg)
{
    _socket->write(QByteArray(msg.toStdString().c_str()));
}

const QByteArray& Bluetooth::cache() const
{
    return _cache;
}

const QBluetoothDeviceDiscoveryAgent* Bluetooth::device_discovery_agent() const
{
    return &_detector;
}

const QBluetoothLocalDevice* Bluetooth::local_device() const
{
    return _local;
}

const QBluetoothSocket* Bluetooth::socket() const
{
    return _socket;
}

const QTimer* Bluetooth::timer() const
{
    return &_timer;
}

const QList<QBluetoothDeviceInfo>& Bluetooth::devices() const
{
    return _devices;
}