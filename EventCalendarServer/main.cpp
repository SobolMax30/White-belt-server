#include "mainwindow.h"

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    QPoint center = QGuiApplication::primaryScreen()->availableGeometry().center();

    w.show();

    QRect frame = w.frameGeometry();
    int x = center.x() - (frame.width()/2);
    int y = center.y() - (frame.height()/2);

    center.setX(x);
    center.setY(y);

    w.move(center);
    return a.exec();
}
