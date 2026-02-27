#include "packetsender.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QSysInfo>

PacketSender::PacketSender(QObject *parent) : QObject(parent) {
    _udpSocket = new QUdpSocket(this);
    _multicastAddress = QHostAddress("239.255.0.1");
    _multicastPort1 = 45454;
    _enabled = false;
}

bool PacketSender::sendEventData(const QDate &date, const QString &text) {
    if (!_enabled) {
        return false;
    }

    QJsonObject doc;
    doc["type"] = "event_added";
    doc["date"] = date.toString("yyyy-MM-dd");
    doc["event"] = text;
    doc["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    doc["sender"] = QSysInfo::machineHostName();

    QByteArray data = QJsonDocument(doc).toJson(QJsonDocument::Compact);
    qint64 bytesSent = _udpSocket->writeDatagram(data, _multicastAddress, _multicastPort1);

    bool success = (bytesSent != -1);
    emit statusUpdate(success);

    return success;
}

void PacketSender::setEnabled(bool enabled) {
    _enabled = enabled;
}
