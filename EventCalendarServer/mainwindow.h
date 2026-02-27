#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include "eventstorage.h"
#include "network.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_Add_clicked();
    void on_pushButton_Remove_clicked();
    void on_pushButton_Find_clicked();
    void on_pushButton_ClearSearch_clicked();
    void on_pushButton_Apply_clicked();
    void on_pushButton_Export_clicked();   
    void on_pushButton_Import_clicked();
    void updateTree();
    void onSyncRequestReceived();
    void statusUpdate(bool success);

private:
    Ui::MainWindow *ui;
    EventStorage *_storage;
    Network *_network;
    bool _networkEnabled;
};

#endif // MAINWINDOW_H
