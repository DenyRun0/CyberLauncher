#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("Cyber Launcher");
    MainWindow w;
    w.show();
    return app.exec();
}
