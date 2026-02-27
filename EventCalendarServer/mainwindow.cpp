#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    _storage = new EventStorage(this);
    _network = new Network(this);
    _networkEnabled = false;

    ui->statusLabel->setFixedSize(20, 20);
    ui->statusLabel->setStyleSheet("border-radius: 10px; border: 1px solid black; background-color: gray;");
    ui->treeWidget->setHeaderLabel("Хронология событий");

    connect(_network, &Network::statusUpdate, this, &MainWindow::statusUpdate);
    connect(_network, &Network::syncRequestReceived, this, &MainWindow::onSyncRequestReceived);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_pushButton_Add_clicked() {
    QString text = ui->lineEdit->text();
    if (!text.isEmpty()) {
        QDate date = ui->dateEdit->date();
        _storage->addEvent(date, text);
        updateTree();

        if (_networkEnabled) {
            _network->sendData(date, text);
        }
        ui->lineEdit->clear();
    }
}

void MainWindow::on_pushButton_Remove_clicked() {
    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
    if (!currentItem) return;

    QString dateText = currentItem->parent() ? currentItem->parent()->text(0) : currentItem->text(0);
    QString eventText = currentItem->parent() ? currentItem->text(0) : QString();

    QString message = eventText.isEmpty()
        ? "Do you really want to remove all events on date: " + dateText + " ?"
        : "Do you really want to remove event: " + eventText + " ?";

    QMessageBox::StandardButton answer = QMessageBox::question(this, "Confirm remove", message, QMessageBox::Yes | QMessageBox::No);

    if (answer == QMessageBox::Yes) {
        _storage->removeItem(dateText, eventText);
        updateTree();
    }
}

void MainWindow::on_pushButton_Find_clicked() {
    ui->treeWidget->clear();
    QStringList events = _storage->findEvents(ui->dateEdit->date());

    for (const QString &event : events) {
        QTreeWidgetItem *eventItem = new QTreeWidgetItem(ui->treeWidget);
        eventItem->setText(0, event);
    }
}

void MainWindow::on_pushButton_ClearSearch_clicked() {
    updateTree();
}

void MainWindow::on_pushButton_Apply_clicked() {
    _networkEnabled = ui->checkBox->isChecked();
    _network->setNetworkEnabled(_networkEnabled);
}

void MainWindow::on_pushButton_Export_clicked() {
    QString filename = QFileDialog::getSaveFileName(this, "Сохранить файл", QDir::homePath() + "/export.txt", "Текстовые файлы (*.txt)");

    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи");
        return;
    }

    QTextStream stream(&file);

    QMap<QDate, QSet<QString>> export_storage = _storage->getAllEvents();

    for (auto it = export_storage.begin(); it != export_storage.end(); ++it) {
        const auto &date = it.key();
        const auto &events = it.value();

        QStringList sortedEvents = events.values();
        std::sort(sortedEvents.begin(), sortedEvents.end());

        for (const QString &event : sortedEvents) {
            stream << date.toString("yyyy-MM-dd") << " | " << event << "\n";
        }
    }

    file.close();

    QMessageBox::information(this, "Успех", "Файл успешно сохранен");
}

void MainWindow::on_pushButton_Import_clicked() {
    QString filename = QFileDialog::getOpenFileName(this, "Открыть файл", QDir::homePath(), "Текстовые файлы (*.txt)");

    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }

    QTextStream stream(&file);

    int addedCount = 0;
    int lineNum = 0;

    while (!stream.atEnd()) {
        QString line = stream.readLine();
        lineNum++;

        QString trimmed_line = line.trimmed();
        if (trimmed_line.isEmpty()) {
            continue;
        }

        QStringList parts = trimmed_line.split(" | ");
        if (parts.size() != 2) {
            continue;
        }

        QString dateString = parts.at(0).trimmed();
        QString eventText = parts.at(1).trimmed();

        QDate date = QDate::fromString(dateString, "yyyy-MM-dd");

        if (date.isValid() && !eventText.isEmpty()) {
            _storage->addEvent(date, eventText);
            addedCount++;
        }
    }

    file.close();

    if (addedCount > 0) {
        updateTree();
        QMessageBox::information(this, "Успех", "События успешно импортированы");
    } else {
        QMessageBox::warning(this, "Предупреждение", "Не найдено событий для импорта");
    }
}

void MainWindow::updateTree() {
    ui->treeWidget->clear();
    QMap<QDate, QSet<QString>> allEvents = _storage->getAllEvents();

    for (auto it = allEvents.begin(); it != allEvents.end(); ++it) {
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

void MainWindow::onSyncRequestReceived() {
    if (_networkEnabled) {
        QMap<QDate, QSet<QString>> allEvents = _storage->getAllEvents();
        for (auto it = allEvents.begin(); it != allEvents.end(); ++it) {
            for (const QString &event : it.value()) {
                _network->sendData(it.key(), event);
            }
        }
    }
}

void MainWindow::statusUpdate(bool success) {
    QString baseStyle = "border-radius: 10px; border: 1px solid black;";
    ui->statusLabel->setStyleSheet(baseStyle + (success ? "background-color: green;" : "background-color: red;"));
    QTimer::singleShot(1000, [this, baseStyle]() {
        ui->statusLabel->setStyleSheet(baseStyle + "background-color: gray;");
    });
}
