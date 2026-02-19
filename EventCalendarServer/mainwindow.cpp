#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QTimer>
#include <QDate>

QString _baseStyle = "border-radius: 10px; border: 1px solid black;";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    _udpSocket = new QUdpSocket(this);
    _multicastAddress = QHostAddress("239.255.0.1");
    _multicastPort = 45454;
    _networkEnabled = false;

    ui->statusLabel->setFixedSize(20, 20);
    ui->statusLabel->setStyleSheet(_baseStyle + "background-color: gray;");
    ui->treeWidget->setHeaderLabel("Хронология событий");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_pushButton_Add_clicked()
{
    addEvent();
}

void MainWindow::on_pushButton_Remove_clicked()
{
    removeEvent();
}

void MainWindow::on_pushButton_Find_clicked()
{
    findEvents();
}

void MainWindow::on_pushButton_ClearSearch_clicked()
{
    updateTree();
}

void MainWindow::on_pushButton_Apply_clicked()
{
    if (ui->checkBox->isChecked()) {
        _networkEnabled = true;
    } else {
        _networkEnabled = false;
    }
}

void MainWindow::addEvent() {
    QString text = ui->lineEdit->text();

    if (!text.isEmpty()) {
        QDate date = ui->dateEdit->date();
        _storage[date].insert(text);

        updateTree();

        if (_networkEnabled) {
            sendData(date, text);
        }
        ui->lineEdit->clear();
    }
}

void MainWindow::removeEvent() {
    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();

    if (!currentItem) return;

    if (currentItem->parent()) {
        QString eventText = currentItem->text(0);
        QString dateText = currentItem->parent()->text(0);
        QDate date = QDate::fromString(dateText, "yyyy-MM-dd");

        if (_storage.contains(date)) {
            QMessageBox::StandardButton answer = QMessageBox::question(this, "Confirm remove",
                    "Do you really want to remove event: " + eventText + " ?", QMessageBox::Yes | QMessageBox::No);

            if (answer == QMessageBox::Yes) {
                _storage[date].remove(eventText);
                if (_storage[date].isEmpty()) {
                    _storage.remove(date);
                }
            }
        }
    }
    else {
        QString dateText = currentItem->text(0);
        QMessageBox::StandardButton answer = QMessageBox::question(this, "Confirm remove",
                "Do you really want to remove all events on date: " + dateText + " ?", QMessageBox::Yes | QMessageBox::No);
        if (answer == QMessageBox::Yes) {
            QDate date = QDate::fromString(dateText, "yyyy-MM-dd");
            _storage.remove(date);
        }
    }

    updateTree();
}

void MainWindow::findEvents() {
    ui->treeWidget->clear();

    QDate date = ui->dateEdit->date();

    if (date.isValid()) {
        if (_storage[date].size() != 0) {
            QStringList eventsList = _storage[date].values();
            eventsList.sort();

            for (const QString &event : eventsList) {
                QTreeWidgetItem *eventItem = new QTreeWidgetItem(ui->treeWidget);
                eventItem->setText(0, event);
            }
        } else {
            _storage.remove(date);
        }
    }
}

void MainWindow::updateTree() {
    ui->treeWidget->clear();

    for (auto it = _storage.begin(); it != _storage.end(); ++it) {
        QTreeWidgetItem *dateItem = new QTreeWidgetItem(ui->treeWidget);
        dateItem->setText(0, it.key().toString("yyyy-MM-dd"));

        QStringList events = it.value().values();
        events.sort();

        for (const QString &event : events) {
            QTreeWidgetItem *eventItem = new QTreeWidgetItem(dateItem);
            eventItem->setText(0, event);
        }
        dateItem->setExpanded(true);
    }
}

void MainWindow::resetStatus() {
    ui->statusLabel->setStyleSheet(_baseStyle + "background-color: gray;");
}

void MainWindow::updateStatusIndicator(bool success) {
    if (success) {
        ui->statusLabel->setStyleSheet(_baseStyle + "background-color: green;");
    } else {
        ui->statusLabel->setStyleSheet(_baseStyle + "background-color: red;");
    }

    QTimer::singleShot(1000, this, &MainWindow::resetStatus);
}

void MainWindow::sendData(const QDate &date, const QString &text) {
    QJsonObject doc;
    doc["type"] = "event_added";
    doc["date"] = date.toString("yyyy-MM-dd");
    doc["event"] = text;
    doc["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    doc["sender"] = QSysInfo::machineHostName();

    QByteArray data = QJsonDocument(doc).toJson(QJsonDocument::Compact);

    qint64 bytesSent = _udpSocket->writeDatagram(data, _multicastAddress, _multicastPort);

    if (!QJsonDocument(doc).isNull()) {
        qDebug().noquote() << "Отправлено: " << QJsonDocument(doc).toJson(QJsonDocument::Indented);
    }

    updateStatusIndicator(bytesSent != -1);
}
