#include "network.h"

Network::Network(QObject *parent) : QObject(parent) {
    _sender = new PacketSender(this);
    _receiver = new PacketReceiver(this);

    connect(_sender, &PacketSender::statusUpdate, this, &Network::statusUpdate);
    connect(_receiver, &PacketReceiver::syncRequestReceived, this, &Network::syncRequestReceived);
}

void Network::sendData(const QDate &date, const QString &text) {
    _sender->sendEventData(date, text);
}

void Network::setNetworkEnabled(bool enabled) {
    _sender->setEnabled(enabled);
    _receiver->setEnabled(enabled);
}
