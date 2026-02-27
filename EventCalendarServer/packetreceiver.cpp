#include "packetreceiver.h"
#include <QDebug>

PacketReceiver::PacketReceiver(QObject *parent) : QObject(parent) {
    _syncSocket = new QUdpSocket(this);
    _multicastAddress = QHostAddress("239.255.0.1");
    _multicastPort2 = 45455;
    _enabled = false;

    _syncSocket->bind(QHostAddress::AnyIPv4, _multicastPort2, QUdpSocket::ShareAddress);
    _syncSocket->joinMulticastGroup(_multicastAddress);

    connect(_syncSocket, &QUdpSocket::readyRead, this, &PacketReceiver::processPendingDatagrams);
}

PacketReceiver::~PacketReceiver() {
    _syncSocket->leaveMulticastGroup(_multicastAddress);
}

void PacketReceiver::processPendingDatagrams() {
    if (!_enabled) {
        return;
    }

    while (_syncSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(_syncSocket->pendingDatagramSize());
        _syncSocket->readDatagram(datagram.data(), datagram.size());

        QString request(datagram);
        if (request == "SYNC_REQUEST") {
            emit syncRequestReceived();
        }
    }
}

void PacketReceiver::setEnabled(bool enabled) {
    _enabled = enabled;
}
