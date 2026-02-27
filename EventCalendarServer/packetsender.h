#ifndef PACKETSENDER_H
#define PACKETSENDER_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QDate>

class PacketSender : public QObject {
    Q_OBJECT

public:
    PacketSender(QObject *parent = nullptr);

    bool sendEventData(const QDate &date, const QString &text);
    void setEnabled(bool enabled);

signals:
    void statusUpdate(bool success);

private:
    QUdpSocket *_udpSocket;
    QHostAddress _multicastAddress;
    quint16 _multicastPort1;
    bool _enabled;
};

#endif // PACKETSENDER_H
