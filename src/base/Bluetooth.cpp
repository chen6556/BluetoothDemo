#include "base/Bluetooth.hpp"

Bluetooth::Bluetooth()
    : _local(new QBluetoothLocalDevice(this)), _server(QBluetoothServiceInfo::RfcommProtocol, this)
{
    _server.setMaxPendingConnections(1);
    _server.listen();

    const char serviceUuid[] = "00001101-0000-1000-8000-00805F9B34FB";

    QBluetoothServiceInfo::Sequence profileSequence;
    QBluetoothServiceInfo::Sequence classId;
    classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::ServiceClassUuid::SerialPort));
    classId << QVariant::fromValue(quint16(0x100));
    profileSequence.append(QVariant::fromValue(classId));
    _server_info.setAttribute(QBluetoothServiceInfo::BluetoothProfileDescriptorList,
                              profileSequence);

    classId.clear();
    classId << QVariant::fromValue(QBluetoothUuid(serviceUuid));
    classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::ServiceClassUuid::SerialPort));

    _server_info.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);

    //! [Service UUID set]
    _server_info.setServiceUuid(QBluetoothUuid(serviceUuid));
    //! [Service UUID set]

    //! [Service Discoverability]
     const QBluetoothUuid groupUuid(QBluetoothUuid::ServiceClassUuid::PublicBrowseGroup);
     QBluetoothServiceInfo::Sequence publicBrowse;
     publicBrowse << QVariant::fromValue(groupUuid);
     _server_info.setAttribute(QBluetoothServiceInfo::BrowseGroupList, publicBrowse);
    //! [Service Discoverability]

    //! [Protocol descriptor list]
    QBluetoothServiceInfo::Sequence protocolDescriptorList;
    QBluetoothServiceInfo::Sequence protocol;
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::ProtocolUuid::L2cap));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    protocol.clear();
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::ProtocolUuid::Rfcomm))
             << QVariant::fromValue(quint8(_server.serverPort()));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    _server_info.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                            protocolDescriptorList);
    //! [Protocol descriptor list]

    //! [Register service]
    _server_info.registerService(_server.serverAddress());

    QObject::connect(&_timer, &QTimer::timeout, &_detector, &QBluetoothDeviceDiscoveryAgent::stop);
    QObject::connect(&_detector, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &Bluetooth::refresh_devices);
    QObject::connect(&_server, &QBluetoothServer::newConnection, this, &Bluetooth::new_connection);
}

Bluetooth::~Bluetooth()
{
    if (_local != nullptr)
    {
        delete _local;
    }
    if (_socket != nullptr)
    {
        _socket->disconnectFromService();
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
    _timer.start(5000);
}

void Bluetooth::connect_to_address(const QBluetoothAddress &address)
{
    if (_socket != nullptr)
    {
        return;
    }
    _socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
    _socket->connectToService(address, QBluetoothUuid("00001101-0000-1000-8000-00805F9B34FB"));
    QObject::connect(_socket, &QBluetoothSocket::disconnected, this, &Bluetooth::reset_socket);
    QObject::connect(_socket, &QBluetoothSocket::readyRead, this, &Bluetooth::read);
    QObject::connect(_socket, &QBluetoothSocket::errorOccurred, this, &Bluetooth::catch_error);
    emit connected();
}

void Bluetooth::disconnect_from_address()
{
    if (_socket == nullptr)
    {
        return;
    }
    _socket->disconnectFromService();
    emit disconnected();
}

void Bluetooth::reset_socket()
{
    QObject::disconnect(_socket, &QBluetoothSocket::disconnected, this, &Bluetooth::reset_socket);
    QObject::disconnect(_socket, &QBluetoothSocket::readyRead, this, &Bluetooth::read);
    QObject::disconnect(_socket, &QBluetoothSocket::errorOccurred, this, &Bluetooth::catch_error);
    _socket->close();
    _socket->deleteLater();
    _socket = nullptr;
}

void Bluetooth::new_connection()
{
    _socket = _server.nextPendingConnection();
    QObject::connect(_socket, &QBluetoothSocket::disconnected, this, &Bluetooth::reset_socket);
    QObject::connect(_socket, &QBluetoothSocket::readyRead, this, &Bluetooth::read);
    QObject::connect(_socket, &QBluetoothSocket::errorOccurred, this, &Bluetooth::catch_error);
    emit connected();
}

void Bluetooth::catch_error(const QBluetoothSocket::SocketError error)
{
    switch (error)
    {
    case QBluetoothSocket::SocketError::RemoteHostClosedError:
        emit disconnected();
        break;
    default:
        break;
    }
}




void Bluetooth::read()
{
    _cache = _socket->readAll();
    emit read_ready();
}

void Bluetooth::write(const QString &msg)
{
    _socket->write(QByteArray(msg.toStdString().c_str()));
}

const QByteArray &Bluetooth::cache() const
{
    return _cache;
}

const QBluetoothDeviceDiscoveryAgent *Bluetooth::device_discovery_agent() const
{
    return &_detector;
}

const QBluetoothLocalDevice *Bluetooth::local_device() const
{
    return _local;
}

const QBluetoothSocket *Bluetooth::socket() const
{
    return _socket;
}

const QTimer *Bluetooth::timer() const
{
    return &_timer;
}

const QList<QBluetoothDeviceInfo> &Bluetooth::devices() const
{
    return _devices;
}