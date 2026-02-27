#include "mainwindow.h"
#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    QPoint center = QGuiApplication::primaryScreen()->availableGeometry().center();
    w.move(center.x() - w.width() / 2, center.y() - w.height() / 2);

    return a.exec();
}
