#include "eventstorage.h"

EventStorage::EventStorage(QObject *parent) : QObject(parent) {}

void EventStorage::addEvent(const QDate &date, const QString &event) {
    if (!event.isEmpty()) {
        _storage[date].insert(event);
    }
}

void EventStorage::removeItem(const QString &dateText, const QString &eventText) {
    QDate date = QDate::fromString(dateText, "yyyy-MM-dd");

    if (eventText.isEmpty()) {
        _storage.remove(date);
    } else {
        if (_storage.contains(date)) {
            _storage[date].remove(eventText);
            if (_storage[date].isEmpty()) {
                _storage.remove(date);
            }
        }
    }
}

QSet<QString> EventStorage::getEvents(const QDate &date) const {
    return _storage.value(date);
}

QMap<QDate, QSet<QString>> EventStorage::getAllEvents() const {
    return _storage;
}

bool EventStorage::contains(const QDate &date) const {
    return _storage.contains(date);
}

bool EventStorage::containsEvent(const QDate &date, const QString &event) const {
    return _storage.contains(date) && _storage[date].contains(event);
}

QStringList EventStorage::findEvents(const QDate &date) const {
    QStringList result;

    if (date.isValid() && _storage.contains(date)) {
        result = _storage[date].values();
        result.sort();
    }

    return result;
}
