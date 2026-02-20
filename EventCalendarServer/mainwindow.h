#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QDate>
#include <QMap>
#include <QSet>

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
    void processSyncRequest();
    void addEvent();
    void removeEvent();
    void updateTree();
    void findEvents();
    void resetStatus();
    void updateStatusIndicator(bool success);
    void sendData(const QDate &date, const QString &text);

private:
    Ui::MainWindow *ui;
    QMap<QDate, QSet<QString>> _storage;
    QUdpSocket *_udpSocket;
    QUdpSocket *_syncSocket;
    QHostAddress _multicastAddress;
    quint16 _multicastPort1;
    quint16 _multicastPort2;
    bool _networkEnabled;
};
#endif // MAINWINDOW_H
