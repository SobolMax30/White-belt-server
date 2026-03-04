#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QDir>

static const QString BASE_STYLE = "border-radius: 10px; border: 1px solid black;";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    _storage = new EventStorage(this);
    _network = new Network(this);
    _fileManager = new FileManager(this);
    _networkEnabled = false;

    ui->statusLabel->setFixedSize(20, 20);
    ui->statusLabel->setStyleSheet(BASE_STYLE + "background-color: gray;");
    ui->treeWidget->setHeaderLabel("Хронология событий");

    connect(_network, &Network::statusUpdate, this, &MainWindow::statusUpdate);
    connect(_network, &Network::syncRequestReceived, this, &MainWindow::onSyncRequestReceived);
    connect(_fileManager, &FileManager::error, this, &MainWindow::showErrorMessage);
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
    if (currentItem == nullptr) {
        return;
    }

    QString dateText;
    QString eventText;

    if (currentItem->parent() != nullptr) {
        dateText = currentItem->parent()->text(0);
        eventText = currentItem->text(0);
    } else {
        dateText = currentItem->text(0);
        eventText = QString();
    }

    QString message;
    if (eventText.isEmpty()) {
        message = "Do you really want to remove all events on date: " + dateText + " ?";
    } else {
        message = "Do you really want to remove event: " + eventText + " ?";
    }

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
    if (ui->checkBox->isChecked()) {
        _networkEnabled = true;
    } else {
        _networkEnabled = false;
    }
    _network->setNetworkEnabled(_networkEnabled);
}

void MainWindow::on_pushButton_Export_clicked() {
    QString filename = QFileDialog::getSaveFileName(this, "Сохранить файл",
        QDir::homePath() + "/export.txt", "Текстовые файлы (*.txt)");

    if (filename.isEmpty()) {
        return;
    }

    bool result = _fileManager->exportToFile(filename, _storage);
    if (result) {
        QMessageBox::information(this, "Успех", "Файл успешно сохранен");
    }
}

void MainWindow::on_pushButton_Import_clicked() {
    QString filename = QFileDialog::getOpenFileName(this, "Открыть файл",
        QDir::homePath(), "Текстовые файлы (*.txt)");

    if (filename.isEmpty()) {
        return;
    }

    int addedCount = 0;
    bool result = _fileManager->importFromFile(filename, _storage, addedCount);

    if (result) {
        if (addedCount > 0) {
            updateTree();
            QString message = "Импортировано " + QString::number(addedCount) + " событий";
            QMessageBox::information(this, "Успех", message);
        } else {
            QMessageBox::warning(this, "Предупреждение", "Не найдено событий для импорта");
        }
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
    if (success) {
        ui->statusLabel->setStyleSheet(BASE_STYLE + "background-color: green;");
    } else {
        ui->statusLabel->setStyleSheet(BASE_STYLE + "background-color: red;");
    }

    QTimer::singleShot(1000, [this, BASE_STYLE]() {
        ui->statusLabel->setStyleSheet(BASE_STYLE + "background-color: gray;");
    });
}

void MainWindow::showErrorMessage(const QString &message) {
    QMessageBox::warning(this, "Ошибка", message);
}
