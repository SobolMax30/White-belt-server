#include "filemanager.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

FileManager::FileManager(QObject *parent) : QObject(parent) {}

bool FileManager::exportToFile(const QString &filename, EventStorage *storage) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit error("Не удалось открыть файл для записи");

        return false;
    }

    QTextStream stream(&file);
    QMap<QDate, QSet<QString>> allEvents = storage->getAllEvents();

    int total = allEvents.size();
    int current = 0;

    for (auto it = allEvents.begin(); it != allEvents.end(); ++it) {
        const QDate &date = it.key();
        const QSet<QString> &events = it.value();

        QStringList sortedEvents = events.values();
        std::sort(sortedEvents.begin(), sortedEvents.end());

        for (const QString &event : sortedEvents) {
            stream << date.toString("yyyy-MM-dd") << " | " << event << "\n";
        }

        current++;
        emit progress(current, total);
    }

    file.close();

    return true;
}

bool FileManager::importFromFile(const QString &filename, EventStorage *storage, int &addedCount) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit error("Не удалось открыть файл");

        return false;
    }

    QTextStream stream(&file);
    addedCount = 0;

    while (!stream.atEnd()) {
        QString line = stream.readLine();

        QDate date;
        QString event;

        if (parseLine(line, date, event)) {
            storage->addEvent(date, event);
            addedCount++;
        }
    }

    file.close();

    return true;
}

bool FileManager::parseLine(const QString &line, QDate &date, QString &event) {
    QString trimmed = line.trimmed();
    if (trimmed.isEmpty()) {
        return false;
    }

    QStringList parts = trimmed.split(" | ");
    if (parts.size() != 2) {
        return false;
    }

    date = QDate::fromString(parts.at(0).trimmed(), "yyyy-MM-dd");
    event = parts.at(1).trimmed();

    if (date.isValid() && !event.isEmpty()) {
        return true;
    } else {
        return false;
    }
}
