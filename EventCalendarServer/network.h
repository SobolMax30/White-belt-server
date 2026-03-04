#ifndef NETWORK_H
#define NETWORK_H

#include "packetsender.h"
#include "packetreceiver.h"

#include <QObject>
#include <QDate>

class Network : public QObject {
    Q_OBJECT

public:
    explicit Network(QObject *parent = nullptr);

    void sendData(const QDate &date, const QString &text);
    void setNetworkEnabled(bool enabled);

signals:
    void statusUpdate(bool success);
    void syncRequestReceived();

private:
    PacketSender *_sender;
    PacketReceiver *_receiver;
};

#endif // NETWORK_H
