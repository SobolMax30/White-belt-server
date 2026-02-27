#ifndef PACKETRECEIVER_H
#define PACKETRECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class PacketReceiver : public QObject {
    Q_OBJECT

public:
    PacketReceiver(QObject *parent = nullptr);
    ~PacketReceiver();

    void setEnabled(bool enabled);

signals:
    void syncRequestReceived();

private slots:
    void processPendingDatagrams();

private:
    QUdpSocket *_syncSocket;
    QHostAddress _multicastAddress;
    quint16 _multicastPort2;
    bool _enabled;
};

#endif // PACKETRECEIVER_H
