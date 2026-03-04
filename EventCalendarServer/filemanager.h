#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "eventstorage.h"

#include <QObject>
#include <QDate>
#include <QSet>

class FileManager : public QObject {
    Q_OBJECT

public:
    explicit FileManager(QObject *parent = nullptr);

    bool exportToFile(const QString &filename, EventStorage *storage);
    bool importFromFile(const QString &filename, EventStorage *storage, int &addedCount);

signals:
    void error(const QString &message);
    void progress(int current, int total);

private:
    bool parseLine(const QString &line, QDate &date, QString &event);
};

#endif // FILEMANAGER_H
