#ifndef EVENTSTORAGE_H
#define EVENTSTORAGE_H

#include <QObject>
#include <QDate>
#include <QMap>
#include <QSet>
#include <QStringList>

class EventStorage : public QObject {
    Q_OBJECT

public:
    EventStorage(QObject *parent = nullptr);

    void addEvent(const QDate &date, const QString &event);
    void removeItem(const QString &dateText, const QString &eventText = QString());
    QSet<QString> getEvents(const QDate &date) const;
    QMap<QDate, QSet<QString>> getAllEvents() const;
    bool contains(const QDate &date) const;
    bool containsEvent(const QDate &date, const QString &event) const;
    QStringList findEvents(const QDate &date) const;

private:
    QMap<QDate, QSet<QString>> _storage;
};

#endif // EVENTSTORAGE_H
